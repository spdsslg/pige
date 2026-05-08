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
using System.Windows.Threading;
using System.Windows.Media;
using System.Windows.Shapes;

namespace WpfPractice2
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public ObservableCollection<Circle> Circles { get; set; }
        private readonly DispatcherTimer _timer;
        private int _progressValue;
        public MainWindow()
        {
            InitializeComponent();

            Circles = new ObservableCollection<Circle>()
            {
                new Circle{Radius = 100 , Time=1},
                new Circle {Radius = 70, Time=1}
            };
            DataContext = this;
            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromMilliseconds(100);
            _timer.Tick += Timer_Tick;

        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            if(_progressValue >= 100)
            {
                DrawCircle();
                _timer.Stop();
                return;
            }
            _progressValue++;
            LoadingProgressBar.Value = _progressValue;
        }

        private void StartProgress(object sender, RoutedEventArgs e)
        {
            _timer.Start();
        }

        private void PauseProgress(object sender, RoutedEventArgs e)
        {
            _timer.Stop();
        }

        private void ResetProgress(object sender, RoutedEventArgs e)
        {
            _timer.Stop();
            _progressValue = 0;
            LoadingProgressBar.Value = _progressValue;
            DrawingCanvas.Children.Clear();
        }

        private void DrawCircle()
        {
            DrawingCanvas.Children.Clear();

            Circle circle = Circles[0];
            double diameter = circle.Radius*2;

            Ellipse ellipse = new Ellipse
            {
                Width = diameter,
                Height = diameter,
                Stroke = Brushes.Gray,
            };

            double leftPadding = (DrawingCanvas.ActualWidth - diameter) / 2;
            double topPadding = (DrawingCanvas.ActualHeight - diameter) / 2;

            Canvas.SetTop(ellipse,topPadding);
            Canvas.SetLeft(ellipse,leftPadding);

            DrawingCanvas.Children.Add(ellipse);
        }

        private void CircleDataGrid_CellEdit(object sender, DataGridCellEditEndingEventArgs e)
        {
            if (_progressValue >= 100)
            {
                Dispatcher.BeginInvoke(new Action(DrawCircle));
            }
        }
    }
}