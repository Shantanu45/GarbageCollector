﻿using GCviz.DllWrappers;
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

        protected void OnPropertyChanged(string prop) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(prop));

    }
}
