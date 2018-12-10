// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// Describes specific categories/subcategories of native fields.
enum NativeFieldFlags : short
{
    // The field may be blittable. The other subcategories determine if the field is blittable.
    NATIVE_FIELD_SUBCATEGORY_MAYBE_BLITTABLE = 1 << 7,
    // The native representation of the field is a floating point field.
    NATIVE_FIELD_SUBCATEGORY_FLOAT = 1 << 8,
    // The field has a nested MethodTable* (i.e. a field of a struct, class, or array)
    NATIVE_FIELD_SUBCATEGORY_NESTED = 1 << 9,
    // The field has a type that is only relevant in COM and requires adherence to the COM ABI.
    NATIVE_FIELD_SUBCATEGORY_COM_ONLY = 1 << 10,
    // The field has a type that is commonly used in COM, but is also usable on non-COM ABIs.
    NATIVE_FIELD_SUBCATEGORY_COM_TYPE = 1 << 11,
    // The native representation of the field can be treated as an integer.
    NATIVE_FIELD_SUBCATEGORY_INTEGER = 1 << 12,
    // The native representation of the field can be treated as an array.
    NATIVE_FIELD_SUBCATEGORY_ARRAY = 1 << 13,
    // The native representation of the field has so special properties.
    NATIVE_FIELD_SUBCATEGORY_OTHER = 1 << 14,
    // The field is a 4-byte floating point.
    NATIVE_FIELD_CATEGORY_R4 = NATIVE_FIELD_SUBCATEGORY_FLOAT | NATIVE_FIELD_SUBCATEGORY_MAYBE_BLITTABLE,
    // The field is an 8-byte floating point
    NATIVE_FIELD_CATEGORY_R8 = NATIVE_FIELD_SUBCATEGORY_FLOAT | NATIVE_FIELD_SUBCATEGORY_MAYBE_BLITTABLE | 0x1,
    // The field is a layout class type (reference type with LayoutKind != LayoutKind.Auto)
    NATIVE_FIELD_CATEGORY_NESTED_LAYOUT_CLASS = NATIVE_FIELD_SUBCATEGORY_NESTED,
    // The field is a value class type
    NATIVE_FIELD_CATEGORY_NESTED_VALUE_CLASS = NATIVE_FIELD_SUBCATEGORY_NESTED & NATIVE_FIELD_SUBCATEGORY_MAYBE_BLITTABLE,
    // The field is a System.DateTime (marshals to a double on Windows)
    NATIVE_FIELD_CATEGORY_DATE = NATIVE_FIELD_SUBCATEGORY_FLOAT | NATIVE_FIELD_SUBCATEGORY_COM_TYPE,
    // The field is marshalled as an in-place (by-value) array.
    NATIVE_FIELD_CATEGORY_IN_PLACE_ARRAY = NATIVE_FIELD_SUBCATEGORY_ARRAY | NATIVE_FIELD_SUBCATEGORY_NESTED,
    // The field should be treated like an integer for the purposes of ABIs.
    NATIVE_FIELD_CATEGORY_INTEGER_LIKE = NATIVE_FIELD_SUBCATEGORY_INTEGER,
    // The field is a blittable integer type.
    NATIVE_FIELD_CATEGORY_BLITTABLE_INTEGER = NATIVE_FIELD_SUBCATEGORY_INTEGER | NATIVE_FIELD_SUBCATEGORY_MAYBE_BLITTABLE,
    // The field is being marshaled to a COM interface pointer.
    NATIVE_FIELD_CATEGORY_INTERFACE_TYPE = NATIVE_FIELD_SUBCATEGORY_COM_ONLY,
    // The field is a structure that is only valid in COM scenarios.
    NATIVE_FIELD_CATEGORY_COM_STRUCT = NATIVE_FIELD_SUBCATEGORY_COM_ONLY | 0x1,
    // The field is a well-known type to the runtime but cannot be treated like an integer when marshalling.
    NATIVE_FIELD_CATEGORY_WELL_KNOWN = NATIVE_FIELD_SUBCATEGORY_OTHER,
    // The field is illegal to marshal.
    NATIVE_FIELD_CATEGORY_ILLEGAL = NATIVE_FIELD_SUBCATEGORY_OTHER | 0x1
};
