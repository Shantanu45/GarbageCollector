using GCviz.DllWrappers;
using GCviz.GCAPIStructs;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace GCviz.Heap
{
    /// <summary>
    /// Interaction logic for Data.xaml
    /// </summary>
    public partial class Data : UserControl, INotifyPropertyChanged
    {
        public Data()
        {
            InitializeComponent();

            DataContext = this;

            bool result = GCAPI.UpdateStats();
            HeapStats stats;

            if (result)
            {
                stats = GCAPIHelpers.GetManagedHeapStats();
                HeapData[] data = GCAPIHelpers.GetHeapDataArray(stats);

                Total = stats.totalSize.ToString();
                Alive = stats.usedLocationsCount.ToString();
                //string name = GCAPIHelpers.MarshalAnsiString(data[0].name);
            }
        }

        private string total = string.Empty;
        public required string Total
        {
            get { return total; }
            set
            {
                if (total != value)
                {
                    total = value;
                    OnPropertyChanged(nameof(Total));
                }
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        protected void OnPropertyChanged(string prop) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));

        private string alive = string.Empty;
        public required string Alive
        {
            get { return alive; }
            set
            {
                if (alive != value)
                {
                    alive = value;
                    OnPropertyChanged(nameof(Alive));
                }
            }
        }
    }
}
