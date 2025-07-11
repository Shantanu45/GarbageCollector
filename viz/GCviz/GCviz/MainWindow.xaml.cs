﻿using GCviz.DllWrappers;
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
            Cleanup();
        }

        private void Cleanup()
        {
            GCAPI.FreeHeapStats();
            GCAPI.FreeGCStats();
        }
    }
}