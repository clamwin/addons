/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIClamwinService.idl
 */

#ifndef __gen_nsIClamwinService_h__
#define __gen_nsIClamwinService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIClamwinService */
#define NS_ICLAMWINSERVICE_IID_STR "84efd804-2e33-4f7c-9318-838dd5014f1a"

#define NS_ICLAMWINSERVICE_IID \
  {0x84efd804, 0x2e33, 0x4f7c, \
    { 0x93, 0x18, 0x83, 0x8d, 0xd5, 0x01, 0x4f, 0x1a }}

/******************
 * Clamwin Service
 ******************/
class NS_NO_VTABLE nsIClamwinService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICLAMWINSERVICE_IID)

  /* readonly attribute ACString Version; */
  NS_IMETHOD GetVersion(nsACString & aVersion) = 0;

  /* void Initialize (); */
  NS_IMETHOD Initialize(void) = 0;

  /* void Log (in string aMessage); */
  NS_IMETHOD Log(const char *aMessage) = 0;

  /* ACString GetTempFilename (in string aOriginalFilename); */
  NS_IMETHOD GetTempFilename(const char *aOriginalFilename, nsACString & _retval) = 0;

  /* ACString GetQuarantineFilename (in string aOriginalFilename); */
  NS_IMETHOD GetQuarantineFilename(const char *aOriginalFilename, nsACString & _retval) = 0;

  /* short ScanFile (in string aPath, out string aReportFile); */
  NS_IMETHOD ScanFile(const char *aPath, char **aReportFile, PRInt16 *_retval) = 0;

  /* void CloseDatabase (); */
  NS_IMETHOD CloseDatabase(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSICLAMWINSERVICE \
  NS_IMETHOD GetVersion(nsACString & aVersion); \
  NS_IMETHOD Initialize(void); \
  NS_IMETHOD Log(const char *aMessage); \
  NS_IMETHOD GetTempFilename(const char *aOriginalFilename, nsACString & _retval); \
  NS_IMETHOD GetQuarantineFilename(const char *aOriginalFilename, nsACString & _retval); \
  NS_IMETHOD ScanFile(const char *aPath, char **aReportFile, PRInt16 *_retval); \
  NS_IMETHOD CloseDatabase(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSICLAMWINSERVICE(_to) \
  NS_IMETHOD GetVersion(nsACString & aVersion) { return _to GetVersion(aVersion); } \
  NS_IMETHOD Initialize(void) { return _to Initialize(); } \
  NS_IMETHOD Log(const char *aMessage) { return _to Log(aMessage); } \
  NS_IMETHOD GetTempFilename(const char *aOriginalFilename, nsACString & _retval) { return _to GetTempFilename(aOriginalFilename, _retval); } \
  NS_IMETHOD GetQuarantineFilename(const char *aOriginalFilename, nsACString & _retval) { return _to GetQuarantineFilename(aOriginalFilename, _retval); } \
  NS_IMETHOD ScanFile(const char *aPath, char **aReportFile, PRInt16 *_retval) { return _to ScanFile(aPath, aReportFile, _retval); } \
  NS_IMETHOD CloseDatabase(void) { return _to CloseDatabase(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSICLAMWINSERVICE(_to) \
  NS_IMETHOD GetVersion(nsACString & aVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVersion(aVersion); } \
  NS_IMETHOD Initialize(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Initialize(); } \
  NS_IMETHOD Log(const char *aMessage) { return !_to ? NS_ERROR_NULL_POINTER : _to->Log(aMessage); } \
  NS_IMETHOD GetTempFilename(const char *aOriginalFilename, nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTempFilename(aOriginalFilename, _retval); } \
  NS_IMETHOD GetQuarantineFilename(const char *aOriginalFilename, nsACString & _retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetQuarantineFilename(aOriginalFilename, _retval); } \
  NS_IMETHOD ScanFile(const char *aPath, char **aReportFile, PRInt16 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ScanFile(aPath, aReportFile, _retval); } \
  NS_IMETHOD CloseDatabase(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CloseDatabase(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsClamwinService : public nsIClamwinService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSICLAMWINSERVICE

  nsClamwinService();

private:
  ~nsClamwinService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsClamwinService, nsIClamwinService)

nsClamwinService::nsClamwinService()
{
  /* member initializers and constructor code */
}

nsClamwinService::~nsClamwinService()
{
  /* destructor code */
}

/* readonly attribute ACString Version; */
NS_IMETHODIMP nsClamwinService::GetVersion(nsACString & aVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Initialize (); */
NS_IMETHODIMP nsClamwinService::Initialize()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Log (in string aMessage); */
NS_IMETHODIMP nsClamwinService::Log(const char *aMessage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* ACString GetTempFilename (in string aOriginalFilename); */
NS_IMETHODIMP nsClamwinService::GetTempFilename(const char *aOriginalFilename, nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* ACString GetQuarantineFilename (in string aOriginalFilename); */
NS_IMETHODIMP nsClamwinService::GetQuarantineFilename(const char *aOriginalFilename, nsACString & _retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* short ScanFile (in string aPath, out string aReportFile); */
NS_IMETHODIMP nsClamwinService::ScanFile(const char *aPath, char **aReportFile, PRInt16 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void CloseDatabase (); */
NS_IMETHODIMP nsClamwinService::CloseDatabase()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIClamwinService_h__ */
