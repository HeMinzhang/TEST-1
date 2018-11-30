using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace tray
{
    /// <summary>
    /// Interaction logic for CubeSelectWindow.xaml
    /// </summary>
    public partial class CubeSelectWindow : Window
    {
        private App app = null;
        private List<IPC.Device> devices = new List<IPC.Device>();
        bool IsLoggedIn = false;

        public CubeSelectWindow()
        {
            InitializeComponent();
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        /// <summary>
        /// Callback for the periodic Status command
        /// </summary>
        /// <param name="status">Status returned by the service</param>
        public void OnStatusRefresh(IPC.ProviderStatus status)
        {
            bool IsLoggedIn_ = false;
            string s = tray.Resources.strings.ipc_error;
            if (status!=null) {
                if (status.status == IPC.ResponseStatus.Error) {
                    btnConnect.IsEnabled = false;
                    btnDisconnect.IsEnabled = false;
                }
                else {
                    IPC.ProviderStatus.providerStatusMessages.TryGetValue(status.providerStatus, out s);
                    if(status.providerStatus == IPC.ProviderStatus.LoggedIn) {
                        IsLoggedIn_ = true;
                    }
                    // enable or disable the connect and disconnect buttons
                    if (status.providerType == IPC.ProviderType.Soft) {
                        btnDisconnect.IsEnabled = false;
                        btnConnect.IsEnabled = false;
                    }
                    else if (status.providerType == IPC.ProviderType.SE3 ||
                        status.providerType == IPC.ProviderType.SECube) {
                        btnDisconnect.IsEnabled = IsLoggedIn_;
                        btnConnect.IsEnabled = !IsLoggedIn_;
                    }
                }
            }
            IsLoggedIn = IsLoggedIn_;
            lblStatus.Text = s;
        }

        private async void btnConnect_Click(object sender, RoutedEventArgs e)
        {
            bool no_device = false, success = false, busy=true;
            byte[] password = null;

            //TODO modal login dialog
            {
                bool? result = null;
                LoginWindow loginWindow = new LoginWindow();
                loginWindow.Owner = this;
                loginWindow.WindowStartupLocation = WindowStartupLocation.CenterOwner;
                result = loginWindow.ShowDialog();

                if (result == null || result == false) {
                    password = new byte[0];
                }
                else {
                    password = loginWindow.Password;
                }
            }

            if(password.Length==0 || password.Length > IPC.PinMax) {
                return;
            }
            

            btnConnect.IsEnabled = false;
            btnDisconnect.IsEnabled = false;

            SELinkTray.Instance.notifyLogin = true;

            // wait for IPC lock
            await Task.Run(() => {
                busy = !app.ipclock.WaitOne(App.IpcTimeout);
            });
            if (!busy)
            {
                byte[] dsn = null;
                byte[] pin = null;

                if (comboDevice.SelectedIndex < 0 || comboDevice.SelectedIndex >= devices.Count) {
                    no_device = true;
                }
                else {
                    success = false;
                    try {
                        pin = password;
                        dsn = devices[comboDevice.SelectedIndex].serialNumber;
                        // issue command
                        await Task.Run(() => {
                            success = app.ipc.SetDevice(dsn, pin);
                        });
                    }
                    catch
                    { }
                }
                app.ipclock.Release();
            }

            btnConnect.IsEnabled = true;
            if (no_device) {
                // should not happen
                MessageBox.Show("NO_DEVICE");
            }
            else if(!success || busy) {
                if (SELinkTray.Instance.notifyLogin) {
                    SELinkTray.Instance.notifyLogin = false;
                    SELinkTray.Instance.Error(tray.Resources.strings.ipc_error);
                }
            }
            else {
                Close();
            }
        }

        /// <summary>
        /// Discover devices asynchronously and update the UI
        /// </summary>
        /// <returns></returns>
        private async Task<bool> RefreshDevices()
        {
            comboDevice.Items.Clear();
            this.devices.Clear();
            bool busy = true;
            await Task.Run(() => {
                busy = !app.ipclock.WaitOne(App.IpcTimeout);
            });
            if (!busy) {
                List<IPC.Device> devices = new List<IPC.Device>();
                await Task.Run(() => {
                    devices = app.ipc.Discover();
                });

                foreach (IPC.Device device in devices) {
                    comboDevice.Items.Add("SECube " + device.SerialNumberAsHex().Substring(0,16) + "... (" + device.root + ")");
                    this.devices.Add(device);
                }
                if (comboDevice.Items.Count > 0) {
                    comboDevice.SelectedIndex = 0;
                }

                btnConnect.IsEnabled = (devices.Count > 0);
                app.ipclock.Release();
            }
            return (!busy);
        }

        private async void Window_Initialized(object sender, EventArgs e)
        {
            app = ((App)Application.Current);

            // restore form's last position
            this.Top = Properties.Settings.Default.CubeSelectLocation.Top;
            this.Left = Properties.Settings.Default.CubeSelectLocation.Left;

            this.Icon = app.icon;

            await RefreshDevices();
        }

        private async void btnRefresh_Click(object sender, RoutedEventArgs e)
        {
            await RefreshDevices();
        }

        private void comboDevice_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try {
                if (comboDevice.SelectedIndex < 0 || comboDevice.SelectedIndex >= devices.Count) return;
                IPC.Device device = devices[comboDevice.SelectedIndex];

                // show selected device's info
                string sn = device.SerialNumberAsHex();
                string[] tmp = new string[16];
                for (int i = 0; i < 16; i++) tmp[i] = sn.Substring(i * 4, 4);
                sn = string.Join(" ", tmp);
                txtCubeInfo.Text = device.root;
                txtCubeSn.Text = sn;
            }
            catch { }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Properties.Settings.Default.CubeSelectLocation = this.RestoreBounds;
            Properties.Settings.Default.Save();
        }

        private async void btnDisconnect_Click(object sender, RoutedEventArgs e)
        {
            bool busy = true;
            await Task.Run(() => {
                busy = !app.ipclock.WaitOne(App.IpcTimeout);
                if (!busy) {
                    app.ipc.Reset();
                    app.ipclock.Release();
                }
            });
            await RefreshDevices();
        }
    }
}
