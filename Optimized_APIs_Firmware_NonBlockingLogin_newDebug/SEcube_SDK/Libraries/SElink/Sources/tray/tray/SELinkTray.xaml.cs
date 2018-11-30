using System;
using System.Threading.Tasks;
using System.Windows;
using System.Drawing;
using System.Reflection;
using System.Windows.Threading;

namespace tray {
    using strings = tray.Resources.strings;

    /// <summary>
    /// persistent hidden form for the notify icon and periodic status query.
    /// </summary>
    public partial class SELinkTray : Window {

        static SELinkTray Instance_ = null;
        bool stopping = true;
        public static SELinkTray Instance {
            get { return Instance_; }
        }

        System.Windows.Forms.NotifyIcon ni = new System.Windows.Forms.NotifyIcon();
        DispatcherTimer dispatcherTimer = null;
        App app = null;

        public bool notifyLogin = false;

        public SELinkTray()
        {
            InitializeComponent();
            Instance_ = this;
        }
        
        private void Window_Initialized(object sender, EventArgs e)
        {
            app = (App)Application.Current;

            // initialize and show notify (tray) icon
            // the notify icon is borrowed from Windows Forms, since there is no WPF equivalent
            ni.MouseUp += new System.Windows.Forms.MouseEventHandler(ni_MouseUp);
            ni.ContextMenuStrip = new System.Windows.Forms.ContextMenuStrip();
            ni.ContextMenuStrip.Items.Add(strings.wt_device, null, MenuDevice_Click);
            ni.ContextMenuStrip.Items.Add(strings.wt_filter_rules, null, MenuRules_Click);
            ni.ContextMenuStrip.Items.Add(strings.wt_quit, null, MenuQuit_Click);
            ni.Icon = tray.Properties.Resources.main_icon;
            ni.Visible = true;
            
            // run periodic task to query the service status
            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += dispatcherTimer_Tick;
            dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
            dispatcherTimer.Start();
            
            this.Icon = app.icon;
        }

        private async void dispatcherTimer_Tick(object sender, EventArgs e)
        {
            IPC.ProviderStatus status = null;

            await Task.Run(() => {
                bool busy = !app.ipclock.WaitOne(300);
                if (!busy) {
                    status= app.ipc.Status();
                    app.SetServiceStatus(status);
                    app.ipclock.Release();
                }
            });

            // show login success or error notification after the user has clicked on Connect
            if(notifyLogin && status != null) {
                if(status.providerStatus != IPC.ProviderStatus.WaitConfig ) {
                    if(status.providerStatus == IPC.ProviderStatus.LoggedIn) {
                        Info(strings.provider_logged_in);
                    }
                    else if(status.providerStatus == IPC.ProviderStatus.ErrorUserPin) {
                        Error(strings.provider_error_userpin);
                    }
                    else {
                        Error(strings.provider_error_login);
                    }
                    notifyLogin = false;
                }
            }
        }

        /// <summary>
        /// Show an error notification balloon
        /// </summary>
        /// <param name="message">error message</param>
        public void Error(string message)
        {
            ni.ShowBalloonTip(2000, strings.wt_balloon_title, 
                message, System.Windows.Forms.ToolTipIcon.Error);
        }
        /// <summary>
        /// Show an info notification ballooon
        /// </summary>
        /// <param name="message">info message</param>
        public void Info(string message)
        {
            ni.ShowBalloonTip(2000, strings.wt_balloon_title, 
                message, System.Windows.Forms.ToolTipIcon.Info);
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        { }

        void MenuDevice_Click(object sender, EventArgs e)
        {
            ((App)Application.Current).ShowCubeSelect();
        }

        void MenuRules_Click(object sender, EventArgs e)
        {
            ((App)Application.Current).ShowFilterRules();
        }

        void MenuQuit_Click(object sender, EventArgs e)
        {
            ((App)Application.Current).CloseWindows();
            stopping = true;
            Close();
        }

        void ni_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            // show context menu on left mouse up
            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                MethodInfo mi = typeof(System.Windows.Forms.NotifyIcon).GetMethod(
                    "ShowContextMenu", BindingFlags.Instance | BindingFlags.NonPublic);
                mi.Invoke(ni, null);
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if(!stopping)e.Cancel = true;
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            ni.Visible = false;
        }
    }
}
