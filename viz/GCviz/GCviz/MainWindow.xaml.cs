using GCviz.DllWrappers;
using GCviz.GCAPIStructs;
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
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

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
            }
        }
    }
}