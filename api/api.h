#ifdef GCAPI_EXPORTS
    #define VIEW_INTERFACE __declspec(dllexport)
  #else
    #define VIEW_INTERFACE __declspec(dllimport)
#endif

VIEW_INTERFACE int GetInt();