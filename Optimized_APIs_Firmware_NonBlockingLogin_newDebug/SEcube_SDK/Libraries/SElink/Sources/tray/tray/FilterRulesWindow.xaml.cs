using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Media;

namespace tray {
    using System.Collections;
    using strings = tray.Resources.strings;

    /// <summary>
    /// Filter rule entry structure for data grid
    /// </summary>
    public class DataGridEntry {
        public DataGridEntry()
        {
            Description = "New";
            Executable = "";
            Address = "";
            PortFirst = "";
            PortLast = "";
            Action = FilterRules.Actions.GetName(FilterRules.Actions.Default);
            KeyId = "";
        }
        public string Description { get; set; }
        public string Executable { get; set; }
        public string Address { get; set; }
        public string PortFirst { get; set; }
        public string PortLast { get; set; }
        public string Action { get; set; }
        public string KeyId { get; set; }
        public override string ToString()
        {
            return Description;
        }
    }

    /// <summary>
    /// Validator for the data grid rows
    /// </summary>
    public sealed class FilterValidationRule : ValidationRule
    {
        public override ValidationResult Validate(
            object value, System.Globalization.CultureInfo cultureInfo)
        {
            DataGridEntry x = (value as BindingGroup).Items[0] as DataGridEntry;

            if (!FilterRules.IsValidRange(x.PortFirst, x.PortLast)) {
                return new ValidationResult(false, strings.wr_val_err_portrange_inval);
            }
            if(!FilterRules.IsValidAddress(x.Address)) {
                return new ValidationResult(false, strings.wr_val_err_address_inval);
            }
            if(!FilterRules.IsValidDescription(x.Description)) {
                return new ValidationResult(false, strings.wr_val_err_description_inval);
            }
            string action = FilterRules.Actions.GetCode(x.Action);
            if (!FilterRules.IsValidAction(action, x.KeyId)) {
                if(action.Equals("encrypt")) {
                    return new ValidationResult(false, strings.wr_val_err_keyid_inval);
                }
                else {
                    return new ValidationResult(false, strings.wr_val_err_keyid_should_be_empty);
                }
            }
            return ValidationResult.ValidResult;
        }
    }


    public partial class FilterRulesWindow : Window {

        List<string> maskWarning = new List<string>();

        /// <summary>
        /// Collection of filter rules bound to the data grid UI element
        /// </summary>
        public ObservableCollection<DataGridEntry> entries { get; set; }

        ContextMenu GridMenu;
        App app = null;
        DataGridDragHelper dragHelper;
        
        public FilterRulesWindow()
        {
            InitializeComponent();
            lblWarn.Visibility = Visibility.Hidden;
        }

        /// <summary>
        /// Callback for the periodic Status command
        /// </summary>
        /// <param name="status">Status returned by the service</param>
        public void OnStatusRefresh(IPC.ProviderStatus status)
        {
            string s = tray.Resources.strings.ipc_error;
            if (status != null) {
                if (status.status != IPC.ResponseStatus.Error) {
                    IPC.ProviderStatus.providerStatusMessages.TryGetValue(status.providerStatus, out s);
                }
            }
            lblStatus.Text = s;
        }

        private void btnCancel_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private string NullToEmpty(int? x)
        {
            if (x == null) return "";
            return x.ToString();
        }
        private string NullToEmpty(string x)
        {
            if (x == null) return "";
            return x;
        }

        private void Window_Initialized(object sender, EventArgs e)
        {
            app = ((App)Application.Current);
            // bind action names as the action cell choices
            Action.ItemsSource = FilterRules.Actions.Names;

            entries = new ObservableCollection<DataGridEntry>();
            this.DataContext = this;
            
            // populate the context menu to show on right mouse click on the data grid
            GridMenu = new ContextMenu();
            {
                MenuItem m = new MenuItem();
                m.Header = strings.wr_insert_before;
                m.Click += new RoutedEventHandler(dataGridMenuInsertBefore_Click);
                GridMenu.Items.Add(m);
            }
            {
                MenuItem m = new MenuItem();
                m.Header = strings.wr_insert_after;
                m.Click += new RoutedEventHandler(dataGridMenuInsertAfter_Click);
                GridMenu.Items.Add(m);
            }
            {
                MenuItem m = new MenuItem();
                m.Header = strings.wr_delete;
                m.Click += new RoutedEventHandler(dataGridMenuDelete_Click);
                GridMenu.Items.Add(m);
            }
            dataGrid.ContextMenu = GridMenu;

            // read filter rules from the configuration file
            FilterRules fl = new FilterRules();
            fl.Read();

            // populate the data grid
            foreach(var rule in fl.conf.Rules) {
                DataGridEntry dge = new DataGridEntry();
                string action = NullToEmpty(rule.Action);
                dge.Action = FilterRules.Actions.GetName(action);
                dge.Address = NullToEmpty(rule.Address);
                dge.Description = NullToEmpty(rule.Description);
                dge.Executable = NullToEmpty(rule.ExePath);
                dge.KeyId = NullToEmpty(rule.KeyId);
                dge.PortFirst = NullToEmpty(rule.PortFirst);
                dge.PortLast = NullToEmpty(rule.PortLast);
                entries.Add(dge);
            }

            // restore form's last state
            this.Top = Properties.Settings.Default.Location.Top;
            this.Left = Properties.Settings.Default.Location.Left;
            this.Width = Properties.Settings.Default.Location.Width;
            this.Height = Properties.Settings.Default.Location.Height;
            if (Properties.Settings.Default.State != WindowState.Minimized) {
                this.WindowState = Properties.Settings.Default.State;
            }

            // bind validation rules to the data grid
            FilterValidationRule validationRule = new FilterValidationRule();
            validationRule.ValidationStep = ValidationStep.UpdatedValue;
            dataGrid.RowValidationRules.Add(validationRule);

            dragHelper = new DataGridDragHelper(gridRoot, popupDrag, popupDragText, dataGrid);

            this.Icon = app.icon;
        }

        /// <summary>
        /// Insert a new empty row, with a default description string
        /// </summary>
        /// <param name="index">position of the new row</param>
        private void InsertNewEntry(int index)
        {
            DataGridEntry dge = new DataGridEntry();

            // find the last description string matching the default template
            int lastnew = 0;
            foreach(var e in entries) {
                int n = 0;
                if (e.Description.Length >= strings.wr_prefix_new.Length + 3 && 
                    e.Description.StartsWith(strings.wr_prefix_new+" #"))
                {
                    if(int.TryParse(e.Description.Substring(
                        strings.wr_prefix_new.Length + 2), out n))
                    {
                        if (lastnew < n) lastnew = n;
                    }
                }
            }
            // fill description field with the next default value
            dge.Description = strings.wr_prefix_new + " #" + (1+lastnew).ToString();
            entries.Insert(index, dge);
            RefreshMaskWarning();
        }

        private void dataGridMenuInsertBefore_Click(object sender, EventArgs e)
        {
            int index = dataGrid.SelectedIndex;
            if (index >= 0 && index < entries.Count) {
                InsertNewEntry(index);
            }
            else {
                InsertNewEntry(dataGrid.Items.Count-1);
            }
        }
        private void dataGridMenuInsertAfter_Click(object sender, EventArgs e)
        {
            int index = dataGrid.SelectedIndex;
            if (index >= 0 && index < entries.Count) {
                InsertNewEntry(index + 1);
            }
            else {
                InsertNewEntry(dataGrid.Items.Count-1);
            }
        }

        private void dataGridMenuDelete_Click(object sender, EventArgs e)
        {
            int index = dataGrid.SelectedIndex;
            if (index >= 0 && index < entries.Count) {
                entries.RemoveAt(index);
                RefreshMaskWarning();
            }
        }

        private async void btnApply_Click(object sender, RoutedEventArgs e)
        {   
            FilterRules fl = new FilterRules();

            foreach (DataGridEntry dge in entries) {
                string action = FilterRules.Actions.GetCode(dge.Action);
                if(!fl.Add(dge.Description, dge.Executable, dge.Address, 
                    dge.PortFirst, dge.PortLast, action, dge.KeyId))    
                {
                    MessageBox.Show(strings.wr_grid_inval, 
                        "SELink", MessageBoxButton.OK, MessageBoxImage.Error);        
                    return;
                }
            }

            bool success = false;
            btnApply.IsEnabled = false;
            await Task.Run(() =>
            {
                fl.Save();
                bool busy = !app.ipclock.WaitOne(App.IpcTimeout);
                if(!busy) {
                    success = app.ipc.Reload();
                    app.ipclock.Release();
                }
            });
            if(!success) {
                SELinkTray.Instance.Error(strings.ipc_error);
            }
            else {
                SELinkTray.Instance.Info(strings.wr_apply_ok);
            }
            btnApply.IsEnabled = true;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // save the form's position
            Properties.Settings.Default.State = this.WindowState;
            Properties.Settings.Default.Location = this.RestoreBounds;
            Properties.Settings.Default.Save();
        }

        /// <summary>
        /// Check whether a rule is masked by another (matches
        /// a subset of the other rule)
        /// </summary>
        /// <param name="cur">masked rule</param>
        /// <param name="prev">masking rule</param>
        /// <returns>true if cur is masked by prev</returns>
        private bool IsMaskedRule(DataGridEntry cur, DataGridEntry prev)
        {
            bool exeMasked = (prev.Executable.Length == 0 || prev.Executable.Equals(cur.Executable));

            int curPortFirst, curPortLast, prevPortFirst, prevPortLast;
            bool portMasked = false;
            if(prev.PortFirst.Length == 0 && prev.PortLast.Length == 0) {
                portMasked = true;
            }
            else if (int.TryParse(cur.PortFirst, out curPortFirst) && int.TryParse(cur.PortLast, out curPortLast) &&
                int.TryParse(prev.PortFirst, out prevPortFirst) && int.TryParse(prev.PortLast, out prevPortLast))
            {
                portMasked= !(prevPortLast < curPortFirst || prevPortFirst > curPortLast); 
            }
            
            bool addressMasked = (prev.Address.Length == 0 || prev.Address.Equals(cur.Address));

            return exeMasked && portMasked && addressMasked;
        }

        /// <summary>
        /// Find all masked rules and generate a list of warnings for the user
        /// </summary>
        private void RefreshMaskWarning()
        {
            maskWarning.Clear();
            for (int i = 1; i < entries.Count; i++) {
                for (int j = 0; j < i; j++) {
                    if (IsMaskedRule(entries[i], entries[j])) {
                        maskWarning.Add(
                            string.Format(
                                strings.wr_masking_warn,
                                i, entries[i].Description, j, entries[j].Description));
                        break;
                    }
                }
            }

            // show warning(s)
            if (maskWarning.Count > 0) {
                lblWarn.Visibility = Visibility.Visible;
                lblWarnContent.Text = maskWarning[0];
                lblWarnContent.TextDecorations = null;
                lblWarn.Cursor = System.Windows.Input.Cursors.Arrow;
                if (maskWarning.Count > 1) {
                    lblWarnContent.Text += string.Format(" "+strings.wr_masking_warn_more, maskWarning.Count - 1);
                    lblWarnContent.TextDecorations = TextDecorations.Underline;
                    lblWarn.Cursor = System.Windows.Input.Cursors.Hand;
                }
            }
            else {
                lblWarn.Visibility = Visibility.Hidden;
            }
        }

        private void lblWarn_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (maskWarning.Count > 1) {
                MessageBox.Show(string.Join("\n", maskWarning), strings.wr_masking_msgbox_title);
            }
        }



        private bool dataGridDirty = false;
        private void dataGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if ( dragHelper.dirty || dataGridDirty) {
                dataGridDirty = false;
                // if the event was fired after editing a cell, recheck masked cells
                RefreshMaskWarning();
            }
        }

        private void dataGrid_CellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            // a cell has been edited
            dataGridDirty = true;
        }

    }

}
