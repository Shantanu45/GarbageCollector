﻿using GCviz.GCAPIStructs;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GCviz.GCAPIStructs
{
    enum GCTimerID
    {
        Pause,
        Mark,
        Sweep,
        Compact,
        Copy,
        Count // Always last!
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct GCTimingStats
    {
        public IntPtr data;
        public uint size;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GCStats
    {
        public uint total;
        public uint alive;
        public uint reclaimed;
        // Pointer to null-terminated ANSI string
        public IntPtr name;
        // Pointer to null-terminated ANSI string
        public IntPtr allocatorName;

        public IntPtr benchmarkData;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct HeapData
    {
        public uint from;

        public uint size;

        // Pointer to null-terminated ANSI string
        public IntPtr name;

        [MarshalAs(UnmanagedType.I1)] // bool is 1 byte in C++
        public bool deleted;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct HeapStats
    {
        public uint totalSize;
        public uint freeSpace;
        public IntPtr usedLocations; // pointer to HeapData array
        public uint usedLocationsCount;
    }
}

namespace GCviz.DllWrappers
{
    static class GCAPI
    {
        private const string _gcDll = "API.dll";

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool RunSample();

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool UpdateStats();

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetHeapStats();

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool FreeHeapStats();

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetGCStats();

        [DllImport(_gcDll, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool FreeGCStats();

    }

    public static class GCAPIHelpers
    {
        public static HeapStats GetManagedHeapStats()
        {
            IntPtr ptr = GCAPI.GetHeapStats();
            if (ptr == IntPtr.Zero)
                throw new InvalidOperationException("Failed to retrieve HeapStats.");

            var stats = Marshal.PtrToStructure<HeapStats>(ptr);

            // Optionally free now or let caller do it
            //GCAPI.FreeHeapStats();

            return stats;
        }

        public static HeapData[] GetHeapDataArray(HeapStats stats)
        {
            if (stats.usedLocations == IntPtr.Zero || stats.usedLocationsCount == 0)
                return Array.Empty<HeapData>();

            HeapData[] result = new HeapData[stats.usedLocationsCount];
            int structSize = Marshal.SizeOf<HeapData>();

            for (int i = 0; i < result.Length; i++)
            {
                IntPtr itemPtr = IntPtr.Add(stats.usedLocations, i * structSize);
                result[i] = Marshal.PtrToStructure<HeapData>(itemPtr);
            }

            return result;
        }

        public static GCStats GetManagedGCStats()
        {
            IntPtr ptr = GCAPI.GetGCStats();
            if (ptr == IntPtr.Zero)
                throw new InvalidOperationException("Failed to retrieve GCStats.");

            var stats = Marshal.PtrToStructure<GCStats>(ptr);

            //GCAPI.FreeGCStats();

            return stats;
        }

        public static double[] GetManagedGCTimingData(GCStats stats)
        {
            if (stats.benchmarkData == IntPtr.Zero)
                return Array.Empty<double>();

            GCTimingStats data = Marshal.PtrToStructure<GCTimingStats>(stats.benchmarkData);

            double[] managedTimingArray = new double[data.size];
            if (data.data != IntPtr.Zero && data.size > 0)
            {
                Marshal.Copy(data.data, managedTimingArray, 0, (int)data.size);
            }

            return managedTimingArray;

        }

        public static string? MarshalAnsiString(IntPtr ptr)
        {
            return ptr == IntPtr.Zero ? null : Marshal.PtrToStringAnsi(ptr);
        }
    }
}
