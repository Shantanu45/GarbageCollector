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
        private string? _value;
        public string? Title { get; set; }

        public string? Value
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
        public MainWindow()
        {
            GCAPI.RunSample();

            InitializeComponent();

            this.Closed += MainWindow_Closed;
        }


        private void MainWindow_Closed(object sender, EventArgs e)
        {
            // Your cleanup code here
            Cleanup();
        }

        private void Cleanup()
        {
            GCAPI.FreeHeapStats();
            GCAPI.FreeGCStats();
            // Code to run when window is destroyed
        }
    }
}