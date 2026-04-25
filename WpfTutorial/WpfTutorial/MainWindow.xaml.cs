using System.Collections.ObjectModel;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfTutorial
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private ObservableCollection<Contact> contacts { get; set; }
        public MainWindow()
        {
            InitializeComponent();
            contacts = new ObservableCollection<Contact>();
            DataContext = contacts;
        }

        private void MenuItem_Exit(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void MenuItem_AddContact(object sender, RoutedEventArgs e)
        {
            Opacity = 0.5;

            AddContactWindow contactWindow = new AddContactWindow();
            if (contactWindow.ShowDialog().Value == true)
            {
                contacts.Add(contactWindow.NewContact);
            }

            Opacity = 1.0;
        }

        private void MenuItem_ClearContacts(object sender, RoutedEventArgs e)
        {
            contacts.Clear();
        }

        private void MenuItem_About(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("This is simple contacts manager.", "Contact Manager",MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }
}