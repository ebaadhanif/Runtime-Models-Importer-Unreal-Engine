// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "RuntimeModelsImporter/Public/AssimpRuntime3DModelsImporter.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAssimpRuntime3DModelsImporter() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
ENGINE_API UClass* Z_Construct_UClass_UMaterial_NoRegister();
ENGINE_API UClass* Z_Construct_UClass_UMaterialInstanceDynamic_NoRegister();
RUNTIMEMODELSIMPORTER_API UClass* Z_Construct_UClass_UAssimpRuntime3DModelsImporter();
RUNTIMEMODELSIMPORTER_API UClass* Z_Construct_UClass_UAssimpRuntime3DModelsImporter_NoRegister();
RUNTIMEMODELSIMPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FModelMeshData();
RUNTIMEMODELSIMPORTER_API UScriptStruct* Z_Construct_UScriptStruct_FModelNodeData();
UPackage* Z_Construct_UPackage__Script_RuntimeModelsImporter();
// End Cross Module References

// Begin ScriptStruct FModelMeshData
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_ModelMeshData;
class UScriptStruct* FModelMeshData::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_ModelMeshData.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_ModelMeshData.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FModelMeshData, (UObject*)Z_Construct_UPackage__Script_RuntimeModelsImporter(), TEXT("ModelMeshData"));
	}
	return Z_Registration_Info_UScriptStruct_ModelMeshData.OuterSingleton;
}
template<> RUNTIMEMODELSIMPORTER_API UScriptStruct* StaticStruct<FModelMeshData>()
{
	return FModelMeshData::StaticStruct();
}
struct Z_Construct_UScriptStruct_FModelMeshData_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --- Mesh Section Info\n" },
#endif
		{ "ModuleRelativePath", "Public/AssimpRuntime3DModelsImporter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "--- Mesh Section Info" },
#endif
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FModelMeshData>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FModelMeshData_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_RuntimeModelsImporter,
	nullptr,
	&NewStructOps,
	"ModelMeshData",
	nullptr,
	0,
	sizeof(FModelMeshData),
	alignof(FModelMeshData),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FModelMeshData_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FModelMeshData_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FModelMeshData()
{
	if (!Z_Registration_Info_UScriptStruct_ModelMeshData.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_ModelMeshData.InnerSingleton, Z_Construct_UScriptStruct_FModelMeshData_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_ModelMeshData.InnerSingleton;
}
// End ScriptStruct FModelMeshData

// Begin ScriptStruct FModelNodeData
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_ModelNodeData;
class UScriptStruct* FModelNodeData::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_ModelNodeData.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_ModelNodeData.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FModelNodeData, (UObject*)Z_Construct_UPackage__Script_RuntimeModelsImporter(), TEXT("ModelNodeData"));
	}
	return Z_Registration_Info_UScriptStruct_ModelNodeData.OuterSingleton;
}
template<> RUNTIMEMODELSIMPORTER_API UScriptStruct* StaticStruct<FModelNodeData>()
{
	return FModelNodeData::StaticStruct();
}
struct Z_Construct_UScriptStruct_FModelNodeData_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// --- Node\n" },
#endif
		{ "ModuleRelativePath", "Public/AssimpRuntime3DModelsImporter.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "--- Node" },
#endif
	};
#endif // WITH_METADATA
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FModelNodeData>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FModelNodeData_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_RuntimeModelsImporter,
	nullptr,
	&NewStructOps,
	"ModelNodeData",
	nullptr,
	0,
	sizeof(FModelNodeData),
	alignof(FModelNodeData),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FModelNodeData_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FModelNodeData_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FModelNodeData()
{
	if (!Z_Registration_Info_UScriptStruct_ModelNodeData.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_ModelNodeData.InnerSingleton, Z_Construct_UScriptStruct_FModelNodeData_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_ModelNodeData.InnerSingleton;
}
// End ScriptStruct FModelNodeData

// Begin Class UAssimpRuntime3DModelsImporter
void UAssimpRuntime3DModelsImporter::StaticRegisterNativesUAssimpRuntime3DModelsImporter()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAssimpRuntime3DModelsImporter);
UClass* Z_Construct_UClass_UAssimpRuntime3DModelsImporter_NoRegister()
{
	return UAssimpRuntime3DModelsImporter::StaticClass();
}
struct Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "AssimpRuntime3DModelsImporter.h" },
		{ "ModuleRelativePath", "Public/AssimpRuntime3DModelsImporter.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_LoadedMaterials_MetaData[] = {
		{ "ModuleRelativePath", "Public/AssimpRuntime3DModelsImporter.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_MasterMaterial_MetaData[] = {
		{ "ModuleRelativePath", "Public/AssimpRuntime3DModelsImporter.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_LoadedMaterials_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_LoadedMaterials;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_MasterMaterial;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAssimpRuntime3DModelsImporter>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_LoadedMaterials_Inner = { "LoadedMaterials", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UClass_UMaterialInstanceDynamic_NoRegister, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_LoadedMaterials = { "LoadedMaterials", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAssimpRuntime3DModelsImporter, LoadedMaterials), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_LoadedMaterials_MetaData), NewProp_LoadedMaterials_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_MasterMaterial = { "MasterMaterial", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAssimpRuntime3DModelsImporter, MasterMaterial), Z_Construct_UClass_UMaterial_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_MasterMaterial_MetaData), NewProp_MasterMaterial_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_LoadedMaterials_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_LoadedMaterials,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::NewProp_MasterMaterial,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_RuntimeModelsImporter,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::ClassParams = {
	&UAssimpRuntime3DModelsImporter::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::PropPointers),
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::Class_MetaDataParams), Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAssimpRuntime3DModelsImporter()
{
	if (!Z_Registration_Info_UClass_UAssimpRuntime3DModelsImporter.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAssimpRuntime3DModelsImporter.OuterSingleton, Z_Construct_UClass_UAssimpRuntime3DModelsImporter_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAssimpRuntime3DModelsImporter.OuterSingleton;
}
template<> RUNTIMEMODELSIMPORTER_API UClass* StaticClass<UAssimpRuntime3DModelsImporter>()
{
	return UAssimpRuntime3DModelsImporter::StaticClass();
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAssimpRuntime3DModelsImporter);
UAssimpRuntime3DModelsImporter::~UAssimpRuntime3DModelsImporter() {}
// End Class UAssimpRuntime3DModelsImporter

// Begin Registration
struct Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_Statics
{
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FModelMeshData::StaticStruct, Z_Construct_UScriptStruct_FModelMeshData_Statics::NewStructOps, TEXT("ModelMeshData"), &Z_Registration_Info_UScriptStruct_ModelMeshData, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FModelMeshData), 367982626U) },
		{ FModelNodeData::StaticStruct, Z_Construct_UScriptStruct_FModelNodeData_Statics::NewStructOps, TEXT("ModelNodeData"), &Z_Registration_Info_UScriptStruct_ModelNodeData, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FModelNodeData), 1569428968U) },
	};
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAssimpRuntime3DModelsImporter, UAssimpRuntime3DModelsImporter::StaticClass, TEXT("UAssimpRuntime3DModelsImporter"), &Z_Registration_Info_UClass_UAssimpRuntime3DModelsImporter, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAssimpRuntime3DModelsImporter), 3366437786U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_3901744356(TEXT("/Script/RuntimeModelsImporter"),
	Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_Statics::ClassInfo),
	Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_PAFMetaVerse_Plugins_RuntimeModelsImporter_Source_RuntimeModelsImporter_Public_AssimpRuntime3DModelsImporter_h_Statics::ScriptStructInfo),
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
