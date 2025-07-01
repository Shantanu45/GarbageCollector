using GCviz.DllWrappers;
using GCviz.GCAPIStructs;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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

namespace GCviz.GC
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
            GCStats gCStats;

            if (result)
            {
                gCStats = GCAPIHelpers.GetManagedGCStats();
                Total = gCStats.total.ToString();
                Alive = gCStats.alive.ToString();
                Reclaimed = gCStats.reclaimed.ToString();
                Collector = GCAPIHelpers.MarshalAnsiString(gCStats.name);
                Allocator = GCAPIHelpers.MarshalAnsiString(gCStats.allocatorName);

                double[] data = GCAPIHelpers.GetManagedGCTimingData(gCStats);
                for (int i = 0; i < data.Length; i++)
                {
                    if (data[i] > 0)
                    {
                        TimingData.Add(new KeyValuePair<string, double>(((GCTimerID)i).ToString() + ":", data[i]));
                    }
                }
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        private string total = string.Empty;

        public string Total
        {
            get { return total; }
            set { 
                if (total != value)
                {
                    total = value;
                    OnPropertyChanged(nameof(Total));
                }
            }
        }

        private string alive = string.Empty;

        public string Alive
        {
            get { return alive; }
            set {
                if (alive != value)
                {
                    alive = value;
                    OnPropertyChanged(nameof(Alive));
                }
            }
        }

        private string reclaimed = string.Empty;

        public string Reclaimed
        {
            get { return reclaimed; }
            set {
                if (reclaimed != value)
                {
                    reclaimed = value;
                    OnPropertyChanged(nameof(Reclaimed));
                }
            }
        }

        private string? collector = string.Empty;

        public string? Collector
        {
            get { return collector; }
            set {
                if (collector != value)
                {
                    collector = value;
                    OnPropertyChanged(nameof(Collector));
                }
            }
        }

        private string? allocator = string.Empty;

        public string? Allocator
        {
            get { return allocator; }
            set
            {
                if (allocator != value)
                {
                    allocator = value;
                    OnPropertyChanged(nameof(Allocator));
                }
            }
        }

        public ObservableCollection<KeyValuePair<string, double>> timingData = new ObservableCollection<KeyValuePair<string, double>>();

        public ObservableCollection<KeyValuePair<string, double>> TimingData
        {
            get { return timingData; }
            set
            {
                if (timingData != value)
                {
                    timingData = value;
                    OnPropertyChanged(nameof(TimingData));
                }
            }
        }

        protected void OnPropertyChanged(string prop) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));

    }
}
