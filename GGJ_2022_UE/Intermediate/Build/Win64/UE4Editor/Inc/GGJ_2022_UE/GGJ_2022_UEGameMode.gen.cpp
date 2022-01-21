// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GGJ_2022_UE/GGJ_2022_UEGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGGJ_2022_UEGameMode() {}
// Cross Module References
	GGJ_2022_UE_API UClass* Z_Construct_UClass_AGGJ_2022_UEGameMode_NoRegister();
	GGJ_2022_UE_API UClass* Z_Construct_UClass_AGGJ_2022_UEGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_GGJ_2022_UE();
// End Cross Module References
	void AGGJ_2022_UEGameMode::StaticRegisterNativesAGGJ_2022_UEGameMode()
	{
	}
	UClass* Z_Construct_UClass_AGGJ_2022_UEGameMode_NoRegister()
	{
		return AGGJ_2022_UEGameMode::StaticClass();
	}
	struct Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_GGJ_2022_UE,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "GGJ_2022_UEGameMode.h" },
		{ "ModuleRelativePath", "GGJ_2022_UEGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AGGJ_2022_UEGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::ClassParams = {
		&AGGJ_2022_UEGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x008802ACu,
		METADATA_PARAMS(Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AGGJ_2022_UEGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AGGJ_2022_UEGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AGGJ_2022_UEGameMode, 201383522);
	template<> GGJ_2022_UE_API UClass* StaticClass<AGGJ_2022_UEGameMode>()
	{
		return AGGJ_2022_UEGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AGGJ_2022_UEGameMode(Z_Construct_UClass_AGGJ_2022_UEGameMode, &AGGJ_2022_UEGameMode::StaticClass, TEXT("/Script/GGJ_2022_UE"), TEXT("AGGJ_2022_UEGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AGGJ_2022_UEGameMode);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
