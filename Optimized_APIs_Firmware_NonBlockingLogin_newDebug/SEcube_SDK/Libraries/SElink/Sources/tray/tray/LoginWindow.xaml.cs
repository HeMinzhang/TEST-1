using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace tray {
    /// <summary>
    /// Interaction logic for LoginWindow.xaml
    /// </summary>
    public partial class LoginWindow : Window {

        private App app;

        private byte[] password = new byte[0];

        public byte[] Password {
            get { return this.password; }
        }

        public LoginWindow()
        {
            InitializeComponent();
        }

        private void txtPassword_PasswordChanged(object sender, RoutedEventArgs e)
        {
            txtPassword.Background = Brushes.White;
        }

        private void Window_ContentRendered(object sender, EventArgs e)
        {
            txtPassword.Focus();
        }

        private void btnLogin_Click(object sender, RoutedEventArgs e)
        {
            byte[] password= Encoding.UTF8.GetBytes(txtPassword.Password);
            if (password.Length==0 || password.Length>IPC.PinMax) {
                this.password = new byte[0];
                txtPassword.Background = Brushes.LightPink;
                return;
            }
            this.password = password;
            DialogResult = true;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = false;
        }

        private void Window_Initialized(object sender, EventArgs e)
        {
            app = ((App)Application.Current);
            this.Icon = app.icon;
        }
    }
}
