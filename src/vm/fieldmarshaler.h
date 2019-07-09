// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.
//
// File: FieldMarshaler.h
//

//
// FieldMarshalers are used to allow CLR programs to allocate and access
// native structures for interop purposes. FieldMarshalers are actually normal GC
// objects with a class, but instead of keeping fields in the GC object,
// it keeps a hidden pointer to a fixed memory block (which may have been
// allocated by a third party.) Field accesses to FieldMarshalers are redirected
// to this fixed block.
//


#ifndef __FieldMarshaler_h__
#define __FieldMarshaler_h__

#include "util.hpp"
#include "mlinfo.h"
#include "eeconfig.h"
#include "olevariant.h"
#include "nativefieldflags.h"

#ifdef FEATURE_COMINTEROP
#endif  // FEATURE_COMINTEROP

#ifdef FEATURE_PREJIT
#include "compile.h"
#endif // FEATURE_PREJIT

// Forward references
class EEClassLayoutInfo;
class FieldDesc;
class MethodTable;

class FieldMarshaler;
class FieldMarshaler_NestedType;
class FieldMarshaler_NestedLayoutClass;
class FieldMarshaler_NestedValueClass;
class FieldMarshaler_StringUni;
class FieldMarshaler_StringAnsi;
class FieldMarshaler_FixedStringUni;
class FieldMarshaler_FixedStringAnsi;
class FieldMarshaler_FixedArray;
class FieldMarshaler_FixedCharArrayAnsi;
class FieldMarshaler_Delegate;
class FieldMarshaler_Illegal;
class FieldMarshaler_Copy1;
class FieldMarshaler_Copy2;
class FieldMarshaler_Copy4;
class FieldMarshaler_Copy8;
class FieldMarshaler_Ansi;
class FieldMarshaler_WinBool;
class FieldMarshaler_CBool;
class FieldMarshaler_Decimal;
class FieldMarshaler_Date;
class FieldMarshaler_BSTR;
#ifdef FEATURE_COMINTEROP
class FieldMarshaler_SafeArray;
class FieldMarshaler_HSTRING;
class FieldMarshaler_Interface;
class FieldMarshaler_Variant;
class FieldMarshaler_VariantBool;
class FieldMarshaler_DateTimeOffset;
class FieldMarshaler_SystemType;
class FieldMarshaler_Exception;
class FieldMarshaler_Nullable;
#endif // FEATURE_COMINTEROP

//=======================================================================
// Each possible CLR/Native pairing of data type has a
// NFT_* id. This is used to select the marshaling code.
//=======================================================================
#undef DEFINE_NFT
#define DEFINE_NFT(name, nativesize, fWinRTSupported) name,
enum NStructFieldType : short
{
#include "nsenums.h"
    NFT_COUNT
};


//=======================================================================
// Magic number for default struct packing size.
//
// Currently we set this to the packing size of the largest supported
// fundamental type and let the field marshaller downsize where needed.
//=======================================================================
#define DEFAULT_PACKING_SIZE 32

enum class ParseNativeTypeFlags : int
{
    None    = 0x00,
    IsAnsi  = 0x01,
#ifdef FEATURE_COMINTEROP
    IsWinRT = 0x02,
#endif // FEATURE_COMINTEROP
};

VOID ParseNativeType(Module*    pModule,
    PCCOR_SIGNATURE             pCOMSignature,
    DWORD                       cbCOMSignature,
    ParseNativeTypeFlags        flags,
    LayoutRawFieldInfo*         pfwalk,
    PCCOR_SIGNATURE             pNativeType,
    ULONG                       cbNativeType,
    mdTypeDef                   cl,
    const SigTypeContext *      pTypeContext,
    BOOL                       *pfDisqualifyFromManagedSequential
#ifdef _DEBUG
    ,
    LPCUTF8                     szNamespace,
    LPCUTF8                     szClassName,
    LPCUTF8                     szFieldName
#endif
);

BOOL IsFieldBlittable(FieldMarshaler* pFM);

//=======================================================================
// This function returns TRUE if the type passed in is either a value class or a class and if it has layout information 
// and is marshalable. In all other cases it will return FALSE. 
//=======================================================================
BOOL IsStructMarshalable(TypeHandle th);

//=======================================================================
// This structure contains information about where a field is placed in a structure, as well as it's size and alignment.
// It is used as part of type-loading to determine native layout and (where applicable) managed sequential layout.
//=======================================================================
struct RawFieldPlacementInfo
{
    UINT32 m_offset;
    UINT32 m_size;
    UINT32 m_alignment;
};

//=======================================================================
// The classloader stores an intermediate representation of the layout
// metadata in an array of these structures. The dual-pass nature
// is a bit extra overhead but building this structure requiring loading
// other classes (for nested structures) and I'd rather keep this
// next to the other places where we load other classes (e.g. the superclass
// and implemented interfaces.)
//
// Each redirected field gets one entry in LayoutRawFieldInfo.
// The array is terminated by one dummy record whose m_MD == mdMemberDefNil.
//=======================================================================
struct LayoutRawFieldInfo
{
    mdFieldDef  m_MD;             // mdMemberDefNil for end of array
    UINT8       m_nft;            // NFT_* value
    RawFieldPlacementInfo m_nativePlacement; // Description of the native field placement
    ULONG       m_sequence;       // sequence # from metadata


    // The LayoutKind.Sequential attribute now affects managed layout as well.
    // So we need to keep a parallel set of layout data for the managed side. The Size and AlignmentReq
    // is redundant since we can figure it out from the sig but since we're already accessing the sig
    // in ParseNativeType, we might as well capture it at that time.
    RawFieldPlacementInfo m_managedPlacement;

    // This field is needs to be 8-byte aligned
    // to ensure that the FieldMarshaler vtable pointer is aligned correctly.
    alignas(8) struct
    {
        private:
            char m_space[MAXFIELDMARSHALERSIZE];
    } m_FieldMarshaler;
};


//=======================================================================
// 
//=======================================================================

VOID LayoutUpdateNative(LPVOID *ppProtectedManagedData, SIZE_T offsetbias, MethodTable *pMT, BYTE* pNativeData, OBJECTREF *ppCleanupWorkListOnStack);
VOID LayoutUpdateCLR(LPVOID *ppProtectedManagedData, SIZE_T offsetbias, MethodTable *pMT, BYTE *pNativeData);
VOID LayoutDestroyNative(LPVOID pNative, MethodTable *pMT);

VOID FmtClassUpdateNative(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData, OBJECTREF *ppCleanupWorkListOnStack);
VOID FmtClassUpdateCLR(OBJECTREF *ppProtectedManagedData, BYTE *pNativeData);
VOID FmtClassDestroyNative(LPVOID pNative, MethodTable *pMT);

VOID FmtValueTypeUpdateNative(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData, OBJECTREF *ppCleanupWorkListOnStack);
VOID FmtValueTypeUpdateCLR(LPVOID pProtectedManagedData, MethodTable *pMT, BYTE *pNativeData);


//=======================================================================
// Abstract base class. Each type of NStruct reference field extends
// this class and implements the necessary methods.
//
//   UpdateNativeImpl
//       - this method receives a COM+ field value and a pointer to
//         native field inside the fixed portion. it should marshal
//         the COM+ value to a new native instance and store it
//         inside *pNativeValue. Do not destroy the value you overwrite
//         in *pNativeValue.
//
//         may throw COM+ exceptions
//
//   UpdateCLRImpl
//       - this method receives a read-only pointer to the native field inside
//         the fixed portion. it should marshal the native value to
//         a new CLR instance and store it in *ppCLRValue.
//         (the caller keeps *ppCLRValue gc-protected.)
//
//         may throw CLR exceptions
//
//   DestroyNativeImpl
//       - should do the type-specific deallocation of a native instance.
//         if the type has a "NULL" value, this method should
//         overwrite the field with this "NULL" value (whether or not
//         it does, however, it's considered a bug to depend on the
//         value left over after a DestroyNativeImpl.)
//
//         must NOT throw a CLR exception
//
//   NativeSizeImpl
//       - returns the size, in bytes, of the native version of the field.
//
//   AlignmentRequirementImpl
//       - returns one of 1,2,4 or 8; indicating the "natural" alignment
//         of the native field. In general,
//
//            for scalars, the AR is equal to the size
//            for arrays,  the AR is that of a single element
//            for structs, the AR is that of the member with the largest AR
//
//
//=======================================================================


#ifndef DACCESS_COMPILE

#define UNUSED_METHOD_IMPL(PROTOTYPE)                   \
    PROTOTYPE                                           \
    {                                                   \
        LIMITED_METHOD_CONTRACT;                                  \
        _ASSERTE(!"Not supposed to get here.");         \
    }

//=======================================================================
//
// FieldMarshaler's are constructed in place and replicated via bit-wise
// copy, so you can't have a destructor. Make sure you don't define a 
// destructor in derived classes!!
// We used to enforce this by defining a private destructor, by the C++
// compiler doesn't allow that anymore.
//
//=======================================================================

class FieldMarshaler
{
    template<typename TFieldMarshaler, typename TSpace, typename... TArgs>
    friend NStructFieldType InitFieldMarshaler(TSpace& space, NativeFieldFlags flags, TArgs&&... args);

public:
    VOID UpdateNative(OBJECTREF* pCLRValue, LPVOID pNativeValue, OBJECTREF *ppCleanupWorkListOnStack) const;
    VOID UpdateCLR(const VOID *pNativeValue, OBJECTREF *ppProtectedCLRValue, OBJECTREF *ppProtectedOldCLRValue) const;
    VOID DestroyNative(LPVOID pNativeValue) const;
    UINT32 NativeSize() const;
    UINT32 AlignmentRequirement() const;
    BOOL IsScalarMarshaler() const;
    BOOL IsNestedValueClassMarshaler() const;
    VOID ScalarUpdateNative(LPVOID pCLR, LPVOID pNative) const;
    VOID ScalarUpdateCLR(const VOID *pNative, LPVOID pCLR) const;
    VOID NestedValueClassUpdateNative(const VOID **ppProtectedCLR, SIZE_T startoffset, LPVOID pNative, OBJECTREF *ppCleanupWorkListOnStack) const;
    VOID NestedValueClassUpdateCLR(const VOID *pNative, LPVOID *ppProtectedCLR, SIZE_T startoffset) const;
    VOID CopyTo(VOID *pDest, SIZE_T destSize) const;
#ifdef FEATURE_PREJIT
    void Save(DataImage *image);
    void Fixup(DataImage *image);
#endif // FEATURE_PREJIT
    void Restore();

    VOID DestroyNativeImpl(LPVOID pNativeValue) const
    {
        LIMITED_METHOD_CONTRACT;
    }

    BOOL IsScalarMarshalerImpl() const
    {
        LIMITED_METHOD_CONTRACT; 
        return FALSE;
    }

    BOOL IsNestedValueClassMarshalerImpl() const
    {
        LIMITED_METHOD_CONTRACT; 
        return FALSE;
    }

    UNUSED_METHOD_IMPL(VOID ScalarUpdateNativeImpl(LPVOID pCLR, LPVOID pNative) const)
    UNUSED_METHOD_IMPL(VOID ScalarUpdateCLRImpl(const VOID *pNative, LPVOID pCLR) const)
    UNUSED_METHOD_IMPL(VOID NestedValueClassUpdateNativeImpl(const VOID **ppProtectedCLR, SIZE_T startoffset, LPVOID pNative, OBJECTREF *ppCleanupWorkListOnStack) const)
    UNUSED_METHOD_IMPL(VOID NestedValueClassUpdateCLRImpl(const VOID *pNative, LPVOID *ppProtectedCLR, SIZE_T startoffset) const)

protected:
    NStructFieldType GetNStructFieldType() const
    {
        LIMITED_METHOD_CONTRACT;
        return m_nft;
    }

private:

    void SetNStructFieldType(NStructFieldType nft)
    {
        LIMITED_METHOD_CONTRACT;
        m_nft = nft;
    }

    void SetNativeFieldFlags(NativeFieldFlags nff)
    {
        LIMITED_METHOD_CONTRACT;
        _ASSERTE(m_nff == 0);
        m_nff = nff;
    }

public:

    BOOL IsIllegalMarshaler() const
    {
        return m_nft == NFT_ILLEGAL ? TRUE : FALSE;
    }

    NativeFieldFlags GetNativeFieldFlags() const
    {
        LIMITED_METHOD_CONTRACT;
        return m_nff;
    }

    // 
    // Methods for saving & restoring in prejitted images:
    //

#ifdef FEATURE_PREJIT
    void SaveImpl(DataImage *image)
    {
        STANDARD_VM_CONTRACT;
    }

    void FixupImpl(DataImage *image)
    {
        STANDARD_VM_CONTRACT;

        image->FixupFieldDescPointer(this, &m_pFD);
    }
#endif // FEATURE_PREJIT

    void RestoreImpl()
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
        }
        CONTRACTL_END;

#ifdef FEATURE_PREJIT
        Module::RestoreFieldDescPointer(&m_pFD);
#endif // FEATURE_PREJIT
    }

    void CopyToImpl(VOID *pDest, SIZE_T destSize) const
    {
        FieldMarshaler *pDestFieldMarshaller = (FieldMarshaler *) pDest;

        _ASSERTE(sizeof(*pDestFieldMarshaller) <= destSize);

        pDestFieldMarshaller->SetFieldDesc(GetFieldDesc());
        pDestFieldMarshaller->SetExternalOffset(GetExternalOffset());
        pDestFieldMarshaller->SetNStructFieldType(GetNStructFieldType());
        pDestFieldMarshaller->SetNativeFieldFlags(GetNativeFieldFlags());
    }

    void SetFieldDesc(FieldDesc* pFD)
    {
        LIMITED_METHOD_CONTRACT;
        m_pFD.SetValueMaybeNull(pFD);
    }

    FieldDesc* GetFieldDesc() const
    {
        CONTRACT (FieldDesc*)
        {
            NOTHROW;
            GC_NOTRIGGER;
            MODE_ANY;
            POSTCONDITION(CheckPointer(RETVAL, NULL_OK));
        }
        CONTRACT_END;

        RETURN m_pFD.GetValueMaybeNull();
    }

    void SetExternalOffset(UINT32 dwExternalOffset)
    {
        LIMITED_METHOD_CONTRACT;
        m_dwExternalOffset = dwExternalOffset;
    }

    UINT32 GetExternalOffset() const
    {
        LIMITED_METHOD_CONTRACT;
        return m_dwExternalOffset;
    }
    
protected:
    FieldMarshaler()
    {
        LIMITED_METHOD_CONTRACT;
        
#ifdef _DEBUG
        m_dwExternalOffset = 0xcccccccc;
#endif
    }

    static inline void RestoreHelper(RelativeFixupPointer<PTR_MethodTable> *ppMT)
    {
        CONTRACTL
        {
            THROWS;
            GC_TRIGGERS;
            MODE_ANY;
            PRECONDITION(CheckPointer(ppMT));
        }
        CONTRACTL_END;

#ifdef FEATURE_PREJIT
        Module::RestoreMethodTablePointer(ppMT);
#else // FEATURE_PREJIT
        // without NGEN we only have to make sure that the type is fully loaded
        MethodTable* pMT = ppMT->GetValue();
        if (pMT != NULL)
            ClassLoader::EnsureLoaded(pMT);
#endif // FEATURE_PREJIT
    }

#ifdef _DEBUG
    static inline BOOL IsRestoredHelper(const RelativeFixupPointer<PTR_MethodTable> &pMT)
    {
        WRAPPER_NO_CONTRACT;

#ifdef FEATURE_PREJIT
        return pMT.IsNull() || (!pMT.IsTagged() && pMT.GetValue()->IsRestored());
#else // FEATURE_PREJIT
        // putting the IsFullyLoaded check here is tempting but incorrect
        return TRUE;
#endif // FEATURE_PREJIT
    }
#endif // _DEBUG

    RelativeFixupPointer<PTR_FieldDesc> m_pFD;      // FieldDesc
    UINT32           m_dwExternalOffset;    // offset of field in the fixed portion
    NStructFieldType m_nft;
    NativeFieldFlags m_nff = (NativeFieldFlags)0;
};

class FieldMarshaler_NestedType : public FieldMarshaler
{
public:
    MethodTable* GetNestedNativeMethodTable() const;
    UINT32 GetNumElements() const;
    UINT32 GetNumElementsImpl() const
    {
        return 1;
    }
};

#endif // DACCESS_COMPILE


#endif // __FieldMarshaler_h__
