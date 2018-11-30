using System;
using System.Drawing;
using System.Threading;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace tray {

    /// <summary>
    /// Application class: coordinate and keep track of windows
    /// </summary>
    public partial class App : Application {

        public IPC ipc = new IPC();
        public Semaphore ipclock = new Semaphore(1,1);
        public const int IpcTimeout = 2000;
        public IPC.ProviderStatus status = null;

        private CubeSelectWindow wndCubeSelect;
        private FilterRulesWindow wndFilterRules;
        private object wndlock = new object();

        public ImageSource icon;

        /// <summary>
        /// Show device selection window
        /// </summary>
        public void ShowCubeSelect()
        {
            lock (wndlock) {
                if(wndCubeSelect == null || !wndCubeSelect.IsVisible) {
                    wndCubeSelect = new CubeSelectWindow();
                    wndCubeSelect.Show();
                }
                else if (!wndCubeSelect.IsActive) {
                    wndCubeSelect.Show();
                    wndCubeSelect.Activate();
                }
            }
        }

        /// <summary>
        /// Show filter rules window
        /// </summary>
        public void ShowFilterRules()
        {
            lock (wndlock) {
                if (wndFilterRules == null || !wndFilterRules.IsVisible) {
                    wndFilterRules = new FilterRulesWindow();
                    wndFilterRules.Show();
                }
                else if (!wndFilterRules.IsActive) {
                    wndFilterRules.Show();
                    wndFilterRules.Activate();
                }
            }
        }

        /// <summary>
        /// Send status information received from the service to all open windows
        /// </summary>
        /// <param name="val">service status response</param>
        public void SetServiceStatus(IPC.ProviderStatus val)
        {
            if (val == null) return;
            status = val;
            lock (wndlock) {
                if(wndFilterRules!=null && wndFilterRules.IsVisible) {
                    wndFilterRules.Dispatcher.InvokeAsync(
                        new Action(delegate { wndFilterRules.OnStatusRefresh(status); }));
                }
                if (wndCubeSelect != null && wndCubeSelect.IsVisible) {
                    wndCubeSelect.Dispatcher.InvokeAsync(
                        new Action(delegate { wndCubeSelect.OnStatusRefresh(status); }));
                }
            }

        }

        /// <summary>
        /// Close all open windows
        /// </summary>
        public void CloseWindows()
        {
            lock (wndlock) {
                if (wndFilterRules != null) {
                    wndFilterRules.Close();
                }
                if (wndCubeSelect != null) {
                    wndCubeSelect.Close();
                }
            }
        }

        private void Application_Startup(object sender, StartupEventArgs e)
        {
            Bitmap bitmap = tray.Properties.Resources.main_icon.ToBitmap();
            IntPtr hBitmap = bitmap.GetHbitmap();
            ImageSource wpfBitmap =
                 Imaging.CreateBitmapSourceFromHBitmap(
                      hBitmap, IntPtr.Zero, Int32Rect.Empty,
                      BitmapSizeOptions.FromEmptyOptions());
            this.icon = wpfBitmap;
        }
    }
}
