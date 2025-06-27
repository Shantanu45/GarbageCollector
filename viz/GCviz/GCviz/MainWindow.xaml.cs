using GCviz.DllWrappers;
using GCviz.GCAPIStructs;
using System.ComponentModel;
using System.Data;
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

namespace GCviz
{
    public class StatItem : INotifyPropertyChanged
    {
        private string _value;
        public string Title { get; set; }

        public string Value
        {
            get => _value;
            set
            {
                if (_value != value)
                {
                    _value = value;
                    OnPropertyChanged(nameof(Value));
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged(string prop) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));
    }
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public StatItem Total { get; set; } = new StatItem { Title = "Total:", Value = "256 MB" };
        public StatItem Alive { get; set; } = new StatItem { Title = "Alive:", Value = "1024" };
        public StatItem Reclaimed { get; set; } = new StatItem { Title = "Reclaimed:", Value = "1024" };
        public MainWindow()
        {
            InitializeComponent();

            DataContext = this;

            GCAPI.RunSample();
            bool result = GCAPI.UpdateStats();
            GCStats gCStats;
            HeapStats heapStats;
            if (result)
            {
                gCStats = GCAPIHelpers.GetManagedGCStats();
                heapStats = GCAPIHelpers.GetManagedHeapStats();
                HeapData[] data = GCAPIHelpers.GetHeapDataArray(heapStats);
                string name = GCAPIHelpers.MarshalAnsiString(data[0].name);

                Total.Value = gCStats.total.ToString();
                Alive.Value = gCStats.alive.ToString();
                Reclaimed.Value = gCStats.reclaimed.ToString();
            }
        }
    }
}