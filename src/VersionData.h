/*! ----------------------------------------------------------------------------------------------------------
@file		VersionData.h
@class		mtbmpi::VersionData
@brief		Application/software product version information.
@details
@internal
project		Master-Task-Blackboard MPI Framework
@author		Thomas E. Hilinski <https://github.com/tehilinski>
@copyright	Copyright 2008, 2020 Thomas E. Hilinski. All rights reserved.
		This software library, including source code and documentation,
		is licensed under the Apache License version 2.0.
		See "LICENSE.md" for more information.
------------------------------------------------------------------------------------------------------------*/

#ifndef INC_mtbmpi_VersionData_h
#define INC_mtbmpi_VersionData_h

#include <string>

namespace mtbmpi {


class VersionData
{
  public:

    /// Constructor accepting std::string arguments
    VersionData (
      std::string const & useProductNameShort,  	///< product's short name
      std::string const & useProductNameLong,   	///< product's long name
      std::string const & useVersionStr,	    	///< version number string
      std::string const & useDescription,	    	///< product description
      std::string const & useCopyright,	    		///< product copyright
      std::string const & useCompanyName,	    	///< company/organization
      std::string const & useProductURL,	    	///< product web address
      std::string const & useSpecialFlags,	    	///< special build flags
      std::string const & useLicenseName,	    	///< product license name
      std::string const & useLicenseURL)	    	///< license web address
      : productNameShort (useProductNameShort),
	productNameLong (useProductNameLong),
	versionStr (useVersionStr),
	description (useDescription),
	copyright (useCopyright),
	companyName (useCompanyName),
	productURL (useProductURL),
	specialFlags (useSpecialFlags),
	licenseName (useLicenseName),
	licenseURL (useLicenseURL)
      {
      }

    /// Constructor accepting char* arguments
    VersionData (
      char const * const useProductNameShort,   	///< product's short name
      char const * const useProductNameLong,    	///< product's long name
      char const * const useVersionStr,	    		///< version number string
      char const * const useDescription,	    	///< product description
      char const * const useCopyright,	    		///< product copyright
      char const * const useCompanyName,	   	///< company/organization
      char const * const useProductURL,	    		///< product web address
      char const * const useSpecialFlags,	    	///< special build flags
      char const * const useLicenseName,	    	///< product license name
      char const * const useLicenseURL)	    		///< license web address
      : productNameShort (useProductNameShort),
	productNameLong (useProductNameLong),
	versionStr (useVersionStr),
	description (useDescription),
	copyright (useCopyright),
	companyName (useCompanyName),
	productURL (useProductURL),
	specialFlags (useSpecialFlags),
	licenseName (useLicenseName),
	licenseURL (useLicenseURL)
      {
      }

    std::string const & ProductNameShort () const { return productNameShort; }	///< product's short name
    std::string const & ProductNameLong ()  const { return productNameLong; }	///< product's long name
    std::string const & VersionStr ()       const { return versionStr; }	///< version number string
    std::string const & Description ()      const { return description; }	///< product description
    std::string const & Copyright ()        const { return copyright; } 	///< product copyright
    std::string const & CompanyName ()      const { return companyName; }	///< company/organization
    std::string const & ProductURL ()       const { return productURL; }	///< product web address
    std::string const & SpecialFlags ()     const { return specialFlags; }	///< special build flags
    std::string const & LicenseName ()      const { return licenseName; }	///< product license name
    std::string const & LicenseURL ()       const { return licenseURL; }	///< license web address

  private:

    /// @cond SKIP_PRIVATE

    std::string const productNameShort;
    std::string const productNameLong;
    std::string const versionStr;
    std::string const description;
    std::string const copyright;
    std::string const companyName;
    std::string const productURL;
    std::string const specialFlags;
    std::string const licenseName;
    std::string const licenseURL;

    // not used
    VersionData (VersionData const & rhs);
    VersionData & operator=(VersionData const & rhs);

    /// @endcond
};


} // namespace mtbmpi

#endif // INC_mtbmpi_VersionData_h
