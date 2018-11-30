using System.Collections;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Controls.Primitives;
using System.Collections.Generic;

namespace tray {
    /// <summary>
    /// Enables moving rows in a DataGrid by drag and drop
    /// </summary>
    class DataGridDragHelper {
        private bool dirty_ = false;
        private bool dragging = false;
        private bool editing = false;
        private DataGrid dataGrid;
        private int dragIndex = -1;
        private System.Windows.Controls.Primitives.Popup popup;
        private TextBlock popupText;
        private UIElement root;

        /// <summary>
        /// Indicates whether a row was moved since the last time this attribute was read
        /// </summary>
        public bool dirty
        {
            get {
                if (dirty_) {
                    dirty_ = false;
                    return true;
                }
                return false;
            }
        }

        /// <summary>
        /// Initialize helper and enable row drag and drop
        /// </summary>
        /// <param name="root">Root UI element of the window</param>
        /// <param name="popup">A Popup UI element to show while dragging</param>
        /// <param name="popupText">A TextBlock element inside the Popup which will be filled
        /// with the item's name</param>
        /// <param name="dataGrid">The DataGrid UI element</param>
        /// <remarks>The DataGrid's ItemSource must implement IList, and its elements must
        /// override ToString, returning the name to be shown in the popup text</remarks>
        public DataGridDragHelper(UIElement root,
            System.Windows.Controls.Primitives.Popup popup,
            TextBlock popupText,
            DataGrid dataGrid)
        {
            this.root = root;
            this.popup = popup;
            this.popupText = popupText;
            this.dataGrid = dataGrid;
            dataGrid.BeginningEdit += OnDataGridBeginningEdit;
            dataGrid.CellEditEnding += OnDataGridCellEditEnding;
            dataGrid.PreviewMouseLeftButtonDown += OnDataGridPreviewMouseLeftButtonDown;
            dataGrid.PreviewMouseLeftButtonUp += OnDataGridPreviewMouseLeftButtonUp;
            root.MouseMove += OnRootMouseMove;
            root.MouseUp += OnRootMouseUp;
        }

        /// <summary>
        /// Cancel drag and drop and hide popup
        /// </summary>
        public void Cancel()
        {
            if (dragging) {
                popup.IsOpen = false;
                dragging = false;
            }
        }

        // ---- Event handlers ----

        private void OnRootMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            Cancel();
        }

        private void OnRootMouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Point mousePosition = e.GetPosition((IInputElement)sender);
            mousePosition.X += 10;
            popup.PlacementRectangle = new Rect(mousePosition, popup.PlacementRectangle.Size);
        }

        private void OnDataGridBeginningEdit(object sender, DataGridBeginningEditEventArgs e)
        {
            editing = true;
            Cancel();
        }

        private void OnDataGridCellEditEnding(object sender, DataGridCellEditEndingEventArgs e)
        {
            editing = false;
        }

        private void OnDataGridPreviewMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (editing) return;
            int index = GetRowIndex(sender, e);
            if (index < 0) return;
            popupText.Text = dataGrid.Items[index].ToString();
            popup.PlacementRectangle = new Rect(e.GetPosition(root), new Size(80, 26));
            popup.IsOpen = true;
            dragIndex = index;
            dragging = true;
        }

        private void OnDataGridPreviewMouseLeftButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            IList itemSource = dataGrid.ItemsSource as IList;
            if (!dragging) return;
            Cancel();
            int targetIndex = GetRowIndex(sender, e);
            if ( targetIndex == itemSource.Count) {
                targetIndex--;
            }
            if (targetIndex < 0 || targetIndex >= itemSource.Count) {
                return;
            }
            if (dragIndex < 0 || dragIndex >= itemSource.Count) {
                return;
            }
            if (targetIndex == dragIndex) return;

            object row = itemSource[dragIndex];

            itemSource.RemoveAt(dragIndex);
            itemSource.Insert(targetIndex, row);
            dirty_ = true;
        }


        // ---- Utilities ----

        /// <summary>
        /// Get the index of the DataGrid row under the mouse pointer
        /// </summary>
        /// <param name="dataGridChild">sender object from any DataGrid mouse event</param>
        /// <param name="e">mouse event arguments</param>
        /// <returns>index of the row under the cursor, or -1 if no row was found</returns>
        private int GetRowIndex(object dataGridChild, System.Windows.Input.MouseButtonEventArgs e)
        {
            int index = -1;
            DependencyObject element = ((UIElement)dataGridChild).InputHitTest(e.GetPosition(dataGrid)) as DependencyObject;
            if (element == null) return -1;
            int loopMax = 20;
            // search for parent of type DataGridRow
            while (element != null && !(element is DataGridRow) && (loopMax-- > 0)) {
                FrameworkElement frameworkElement = element as FrameworkElement;
                if (frameworkElement != null && frameworkElement.Parent != null) {
                    element = frameworkElement.Parent;
                    continue;
                }
                ContentElement contentElement = element as ContentElement;
                if (contentElement != null) {
                    DependencyObject contentElementParent = ContentOperations.GetParent(contentElement);
                    if (contentElementParent != null) {
                        element = contentElementParent;
                        continue;
                    }
                }
                element = VisualTreeHelper.GetParent(element);
            }

            if (element != null) {
                if (element is DataGridRow) {
                    // DataGridRow found
                    index = (element as DataGridRow).GetIndex();
                }
            }
            if (index < 0 || index >= dataGrid.Items.Count) {
                return -1;
            }
            return index;
        }
    }
}
