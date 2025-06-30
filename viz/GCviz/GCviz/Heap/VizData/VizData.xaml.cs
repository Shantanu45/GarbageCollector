using GCviz.DllWrappers;
using GCviz.GC;
using GCviz.GCAPIStructs;
using LiveChartsCore;
using LiveChartsCore.Kernel.Sketches;
using LiveChartsCore.Painting;
using LiveChartsCore.SkiaSharpView;
using LiveChartsCore.SkiaSharpView.Painting;
using SkiaSharp;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using static System.Runtime.InteropServices.JavaScript.JSType;


namespace GCviz.Heap
{
    public struct AllocData
    {
        public uint size;

        // Pointer to null-terminated ANSI string
        public string name;

        public bool deleted;
    }

        /// <summary>
        /// Interaction logic for VizData.xaml
        /// </summary>
    public partial class VizData : UserControl
    {

        public SortedDictionary<uint, AllocData> usedLocations = new SortedDictionary<uint, AllocData>();
        SortedDictionary<uint, AllocData> freeLocations = new SortedDictionary<uint, AllocData>();

        public ISeries[] Series { get; set; }
        public ICartesianAxis[] YAxis { get; set; } = [
            new Axis
            {
                LabelsRotation = -15,
                Labels = ["Heap 1"],
            }
        ];

        public ICartesianAxis[] XAxis { get; set; } = [
            new Axis
            {
                LabelsRotation = 0,
                Name = "Memory (Bytes)",
                DrawTicksPath = false,
                MinStep = 16,
                MinLimit = 0
            }
        ];

        public VizData()
        {
            InitializeComponent();


            DataContext = this;

            bool result = GCAPI.UpdateStats();
            HeapStats stats = new HeapStats();

            if (result)
            {
                stats = GCAPIHelpers.GetManagedHeapStats();
                HeapData[] data = GCAPIHelpers.GetHeapDataArray(stats);

                uint Total = stats.totalSize;


                foreach (HeapData item in data)
                {
                    AllocData allocData = new AllocData
                    {
                        size = item.size,
                        name = GCAPIHelpers.MarshalAnsiString(item.name),
                        deleted = item.deleted
                    };

                    usedLocations.Add(item.from, allocData);
                }

            }

            GenerateFreeList(usedLocations, stats.totalSize);
            var combined = new SortedDictionary<uint, AllocData>(usedLocations);

            foreach (var kvp in freeLocations)
            {
                combined[kvp.Key] = kvp.Value;
            }

            UpdatePlot(combined);

        }

        private void UpdatePlot(SortedDictionary<uint, AllocData> data)
        {

            double[] values = new double[data.Count];

            int index = 0;
            foreach (var kvp in data)
            {
                values[index] = kvp.Value.size;
                index++;
            }

            int rowsCount = 1;

            var seriesList = new List<ISeries>();

            var colors = new[] { SKColors.Coral, SKColors.Cyan, SKColors.Aqua, SKColors.AliceBlue };

            SolidColorPaint fillColor = new SolidColorPaint();

            int segmentIndex = 0;
            foreach (var kvp in data)
            {
                var segmentValues = new List<double>();
                for (int rowIndex = 0; rowIndex < rowsCount; rowIndex++)
                {
                    segmentValues.Add(kvp.Value.size);
                }
                if (kvp.Value.name == "Free")
                {
                    fillColor = new SolidColorPaint(SKColors.Gray);
                }
                else
                {
                    fillColor = new SolidColorPaint(colors[segmentIndex % colors.Length]);
                }

                seriesList.Add(new StackedRowSeries<double>
                {
                    Values = segmentValues,
                    Name = kvp.Value.name,
                    Fill = fillColor,
                    Stroke = new SolidColorPaint(SKColors.Black, 2),

                    DataLabelsPaint = new SolidColorPaint(SKColors.Black),
                    DataLabelsPosition = LiveChartsCore.Measure.DataLabelsPosition.Middle,
                    DataLabelsFormatter = (point) => $"{point.Model.ToString()}B"
                });
                segmentIndex++;
            }


            Series = seriesList.ToArray();

        }

        private void GenerateFreeList(SortedDictionary<uint, AllocData> data, uint TotalSize)
        {
            uint curr = 0;
            uint prev = 0;
            uint mark = 0;
            foreach (var kvp in usedLocations)
            {
                curr = kvp.Key;
                if (curr > prev)
                {
                    mark = prev;
                    AllocData allocData = new AllocData
                    {
                        size = curr - prev,
                        name = "Free",
                        deleted = kvp.Value.deleted,
                    };
                    freeLocations.Add(mark, allocData);
                }
                prev = kvp.Key + kvp.Value.size;
            }
            if (prev < TotalSize)
            {
                AllocData allocData = new AllocData
                {
                    size = TotalSize - prev,
                    name = "Free",
                    deleted = false,
                };
                freeLocations.Add(prev, allocData);
            }
        }
    }
}
