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
using System.Diagnostics;

namespace WpfPractice2
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public ObservableCollection<Circle> Circles { get; set; }
        private readonly Stopwatch _stopwatch = new Stopwatch();
        private readonly List<Point> _penPoints = new List<Point>();

        private const double ProcedureSeconds = 10.0;

        private bool _drawCircles = true;
        private bool _drawLines = true;
        private readonly DispatcherTimer _timer;
        private int _progressValue;
        public MainWindow()
        {
            InitializeComponent();

            Circles = new ObservableCollection<Circle>()
            {
                new Circle{Radius = 100 , Frequency=1},
                new Circle {Radius = 70, Frequency=3}
            };
            DataContext = this;
            _timer = new DispatcherTimer();
            _timer.Interval = TimeSpan.FromMilliseconds(20);
            _timer.Tick += Timer_Tick;

        }

        private void Timer_Tick(object sender, EventArgs e)
        {
            double progress = _stopwatch.Elapsed.TotalSeconds/ProcedureSeconds;
            if(progress >= 1.0)
            {
                progress = 1.0;
                _timer.Stop();
                _stopwatch.Stop();
            }
            
            LoadingProgressBar.Value = progress*100.0;
            DrawFrame(progress);
        }

        private void StartProgress(object sender, RoutedEventArgs e)
        {
            DrawingCanvas.Children.Clear();
            _penPoints.Clear();

            LoadingProgressBar.Value = 0;

            _stopwatch.Restart();
            _timer.Start();
        }

        private void PauseProgress(object sender, RoutedEventArgs e)
        {
            if (_timer.IsEnabled)
            {
                _timer.Stop();
                _stopwatch.Stop();
            }
            else
            {
                _stopwatch.Start();
                _timer.Start();
            }
        }

        private void ResetProgress(object sender, RoutedEventArgs e)
        {
            _timer.Stop();
            _stopwatch.Reset();

            _progressValue = 0;
            LoadingProgressBar.Value = _progressValue;

            _penPoints.Clear();
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

        private void DrawCirclesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            _drawCircles = DrawCirclesMenuItem.IsChecked;
        }

        private void DrawLinesMenuItem_Click(object sender, RoutedEventArgs e)
        {
            _drawLines = DrawLinesMenuItem.IsChecked;
        }

        private void DrawFrame(double progress)
        {
            DrawingCanvas.Children.Clear();

            if (Circles.Count == 0)
                return;

            Point center = new Point(
                DrawingCanvas.ActualWidth / 2.0,
                DrawingCanvas.ActualHeight / 2.0
            );

            Point currentCenter = center;

            foreach (Circle circle in Circles)
            {
                double angle = 2.0 * Math.PI * circle.Frequency * progress;

                Point radiusEnd = new Point(
                    currentCenter.X + circle.Radius * Math.Cos(angle),
                    currentCenter.Y + circle.Radius * Math.Sin(angle)
                );

                if (_drawCircles)
                {
                    DrawHelperCircle(currentCenter, circle.Radius);
                }

                if (_drawLines)
                {
                    DrawRadiusLine(currentCenter, radiusEnd);
                }

                currentCenter = radiusEnd;
            }

            _penPoints.Add(currentCenter);

            DrawCurve();
            DrawPenDot(currentCenter);
        }

        private void DrawHelperCircle(Point center, double radius)
        {
            Ellipse ellipse = new Ellipse
            {
                Width = radius * 2.0,
                Height = radius * 2.0,
                Stroke = Brushes.LightGray,
                StrokeThickness = 1
            };

            Canvas.SetLeft(ellipse, center.X - radius);
            Canvas.SetTop(ellipse, center.Y - radius);

            DrawingCanvas.Children.Add(ellipse);
        }

        private void DrawRadiusLine(Point start, Point end)
        {
            Line line = new Line
            {
                X1 = start.X,
                Y1 = start.Y,
                X2 = end.X,
                Y2 = end.Y,
                Stroke = Brushes.Gray,
                StrokeThickness = 1
            };

            DrawingCanvas.Children.Add(line);
        }

        private void DrawCurve()
        {
            if (_penPoints.Count < 2)
                return;

            Polyline curve = new Polyline
            {
                Stroke = Brushes.Black,
                StrokeThickness = 2
            };

            foreach (Point point in _penPoints)
            {
                curve.Points.Add(point);
            }

            DrawingCanvas.Children.Add(curve);
        }

        private void DrawPenDot(Point position)
        {
            double size = 8;

            Ellipse dot = new Ellipse
            {
                Width = size,
                Height = size,
                Fill = Brushes.Black
            };

            Canvas.SetLeft(dot, position.X - size / 2.0);
            Canvas.SetTop(dot, position.Y - size / 2.0);

            DrawingCanvas.Children.Add(dot);
        }
    }
}