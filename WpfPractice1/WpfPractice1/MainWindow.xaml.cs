using System.Collections.ObjectModel;
using System.Diagnostics.Metrics;
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

namespace WpfPractice1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private ObservableCollection<Book> books { get; set; }
        private static int Counter = 0;
        public MainWindow()
        {
            InitializeComponent();
            books = new ObservableCollection<Book>();
            books.Add(new Book() { Title="Book", Author="Me", Category=BookCategory.Novel, Isbn="112", IsReserved=false, Year=2007});
            DataContext = books;
        }

        private void MenuItem_Exit(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void MenuItem_ClearBooks(object sender, RoutedEventArgs e)
        {
            books.Clear();
        }

        private void MenuItem_AddBook(object sender, RoutedEventArgs e)
        {
            Counter++;
            books.Add(new Book() { Title = $"Book{Counter}", Author = "Me", Category = (BookCategory)(Counter%(Enum.GetValues(typeof(BookCategory)).Length)), Isbn = "112", IsReserved = false, Year = 2007 });
        }

        private void MenuItem_About(object sender, RoutedEventArgs e)
        {
            MessageBox.Show("This is a library app", "About", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void CheckBox_Reserved(object sender, RoutedEventArgs e)
        {
            CollectionViewSource? cvs = this.Resources["LibraryViewSource"] as CollectionViewSource;
            if(cvs is null)
            {
                return;
            }
            CheckBox checkBox = this.ReservedOnlyCheckbox;
            if (checkBox.IsChecked == false)
            {
                cvs.View.Filter = null;
            }

            else
            {
                cvs.View.Filter = item =>
                {
                    if (item is Book book)
                        return (book as Book).IsReserved;
                    return false;
                };
            }
            cvs.View.Refresh();
        }
    }
}