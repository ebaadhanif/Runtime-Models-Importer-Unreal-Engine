// Class Added by Ebaad, This class deals with Model Raw Data Extraction using Assimp , Creating Model From that Data and Spawning on Demand in Scene On Certain Location
#include "AssimpRuntime3DModelsImporter.h"
#include "ProceduralMeshComponent.h"
#include "Engine/World.h"
#include "StaticMeshAttributes.h"
#include "Misc/Paths.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/BodySetup.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "DirectXTex.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "Kismet/GameplayStatics.h"
#include <KismetProceduralMeshLibrary.h>

UAssimpRuntime3DModelsImporter::UAssimpRuntime3DModelsImporter() {
}

void UAssimpRuntime3DModelsImporter::ParseNode(aiNode* Node, const aiScene* Scene, FModelNodeData& OutNode, const FString& FbxFilePath) {
	OutNode.Name = UTF8_TO_TCHAR(Node->mName.C_Str());
	OutNode.Transform = ConvertAssimpMatrix(Node->mTransformation);
	for (uint32 i = 0; i < Node->mNumMeshes; ++i) {
		aiMesh* Mesh = Scene->mMeshes[Node->mMeshes[i]];
		FModelMeshData MeshData;
		ExtractMesh(Mesh, Scene, MeshData, FbxFilePath);
		OutNode.MeshSections.Add(MoveTemp(MeshData));
	}

	for (uint32 i = 0; i < Node->mNumChildren; ++i) {
		FModelNodeData ChildNode;
		ParseNode(Node->mChildren[i], Scene, ChildNode, FbxFilePath);
		OutNode.Children.Add(MoveTemp(ChildNode));
	}
}

void UAssimpRuntime3DModelsImporter::ExtractMesh(aiMesh* Mesh, const aiScene* Scene, FModelMeshData& OutMesh, const FString& FbxFilePath)
{
	const bool bHasNormals = Mesh->HasNormals();
	const bool bHasUVs = Mesh->HasTextureCoords(0);

	// --- Vertices, Normals, UVs ---
	for (uint32 i = 0; i < Mesh->mNumVertices; ++i)
	{
		OutMesh.Vertices.Add(FVector(Mesh->mVertices[i].x, Mesh->mVertices[i].z, Mesh->mVertices[i].y));

		if (bHasNormals)
			OutMesh.Normals.Add(FVector(Mesh->mNormals[i].x, Mesh->mNormals[i].z, Mesh->mNormals[i].y));
		else
			OutMesh.Normals.Add(FVector::UpVector);

		if (bHasUVs)
			OutMesh.UVs.Add(FVector2D(Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y));
		else
			OutMesh.UVs.Add(FVector2D::ZeroVector);
	}
	// --- Tangents & Bi	tangents ---
	if (!Mesh->HasTangentsAndBitangents())
	{
		// Generate tangents using UE's helper
		TArray<FProcMeshTangent> GeneratedTangents;
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(
			OutMesh.Vertices,
			OutMesh.Triangles,
			OutMesh.UVs,
			OutMesh.Normals,
			GeneratedTangents
		);

		// Store them directly in OutMesh.Tangents as FVector (you can convert)
		OutMesh.Tangents.Empty();
		for (const FProcMeshTangent& Tangent : GeneratedTangents)
		{
			OutMesh.Tangents.Add(Tangent.TangentX); // TangentX is FVector
		}

		// No need to store bitangents; UE procedural mesh calculates internally
	}



	// --- Triangles ---
	for (uint32 i = 0; i < Mesh->mNumFaces; ++i)
	{
		const aiFace& Face = Mesh->mFaces[i];
		if (Face.mNumIndices == 3)
		{
			OutMesh.Triangles.Add(Face.mIndices[0]);
			OutMesh.Triangles.Add(Face.mIndices[1]);
			OutMesh.Triangles.Add(Face.mIndices[2]);
		}
	}

	// --- Material assignment ---
	if (Mesh->mMaterialIndex >= 0 && Scene->mMaterials[Mesh->mMaterialIndex])
	{
		OutMesh.Material = CreateMaterialFromAssimp(Scene->mMaterials[Mesh->mMaterialIndex], Scene, FbxFilePath);
	}
}


AActor* UAssimpRuntime3DModelsImporter::SpawnModel(UWorld* World, const FTransform& modelTransform,
	TArray<FString> Lods, FString DamagedName, FString DestroyedName, FString BattlespaceName

)
{
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Invalid world context in SpawnModel"));
		return nullptr;
	}

	// Spawn the root container actor
	AActor* RootActor = World->SpawnActor<AActor>(AActor::StaticClass(), modelTransform);

	if (!RootActor)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to spawn RootActor"));
		return nullptr;
	}
	else
	{
		RootFBXActor = RootActor;

	}

#if WITH_EDITOR
	RootActor->SetActorLabel(ModelName);
#endif

	// Add root scene component
	USceneComponent* RootComp = NewObject<USceneComponent>(RootActor);
	if (!RootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to create RootComponent"));
		RootActor->Destroy();
		return nullptr;
	}

	RootComp->RegisterComponent();
	RootActor->SetRootComponent(RootComp);
	RootActor->SetActorTransform(FTransform(modelTransform));



	// ✅ Spawn the entire hierarchy starting from the real RootNode
	SpawnNodeRecursive(World, RootNode, RootActor,Lods, DamagedName, DestroyedName, BattlespaceName);

	// ✅ Optional debug log
	UE_LOG(LogTemp, Log, TEXT("✅ Spawned model '%s' with %d nodes"), *ModelName, SpawnedNodeActors.Num());

	// Uncomment to list all spawned nodes
	/*
	for (const auto& Pair : SpawnedNodeActors)
	{
		UE_LOG(LogTemp, Log, TEXT("   - Spawned NodeActor: %s"), *Pair.Key);
	}
	*/

	return RootActor;
}

void UAssimpRuntime3DModelsImporter::SpawnNodeRecursive(UWorld* World, const FModelNodeData& Node, AActor* Parent,
	TArray<FString> Lods, FString DamagedName, FString DestroyedName, FString BattlespaceName
)
{
	if (!World || !Parent)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ World or Parent Actor is invalid."));
		return;
	}

	// Spawn Actor safely using deferred spawning
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* NodeActor = World->SpawnActorDeferred<AActor>(AActor::StaticClass(), FTransform::Identity, Parent);
	if (!NodeActor)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to spawn NodeActor for node: %s"), *Node.Name);
		return;
	}

#if WITH_EDITOR
	NodeActor->SetActorLabel(Node.Name);
#endif

	// Attach and setup root component safely
	USceneComponent* RootComp = NewObject<USceneComponent>(NodeActor);
	if (!RootComp)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to create RootComp for node: %s"), *Node.Name);
		NodeActor->Destroy();
		return;
	}

	RootComp->RegisterComponent();
	NodeActor->SetRootComponent(RootComp);
	NodeActor->AttachToActor(Parent, FAttachmentTransformRules::KeepRelativeTransform);
	NodeActor->SetActorRelativeTransform(Node.Transform);

	// Store reference AFTER verifying NodeActor initialization
	SpawnedNodeActors.Add(Node.Name, NodeActor);

	for (int i = 0; i < Lods.Num(); i++)
	{
		LODsMap.Add(i, Lods[i]);
	}

	for (int i = 0; i < Lods.Num(); i++)
	{
		if (Node.Name == Lods[i] && !LODs.Contains(NodeActor))
		{
			LODs.Add(NodeActor);
			TempLODsActors.Add(NodeActor);
			LODsNames.Add(Node.Name);
			Lods.RemoveAt(i);
		}
	}

	/*for (int i = 0; i < Lods.Num(); i++)
	{
		for (int j= 0; j < LODsNames.Num(); j++)
		{
			if (LODsMap[i] == LODsNames[j])
			{
				LODs[i] = TempLODsActors[j];
			}
		}
	}*/

	//if (lod1Name != "" && LOD1_Parent == nullptr && lod1Name == Node.Name)
	//	LOD1_Parent = NodeActor;
	if (DamagedName != "" && DamagedActors_Parent == nullptr && DamagedName == Node.Name)
		DamagedActors_Parent = NodeActor;
	if (DestroyedName != "" && DestroyedActors_Parent == nullptr && DestroyedName == Node.Name)
		DestroyedActors_Parent = NodeActor;
	if (BattlespaceName != "" && Battlespace_Actor == nullptr && BattlespaceName == Node.Name)
		Battlespace_Actor = NodeActor;




	NodeActor->SetActorHiddenInGame(false);
	// Create mesh sections
	for (const FModelMeshData& Section : Node.MeshSections)
	{
		UProceduralMeshComponent* Mesh = NewObject<UProceduralMeshComponent>(NodeActor);
		if (!Mesh)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to create MeshComponent for node: %s"), *Node.Name);
			continue;
		}

		Mesh->RegisterComponent();
		Mesh->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
		NodeActor->AddInstanceComponent(Mesh);

		// --- Convert FVector tangents to FProcMeshTangent ---
		TArray<FProcMeshTangent> ProcTangents;
		for (const FVector& TangentVec : Section.Tangents)
		{
			// true = Flip Y to match UE coordinate system (tangent space)
			ProcTangents.Add(FProcMeshTangent(TangentVec, true));
		}

		// --- Create mesh section ---
		Mesh->CreateMeshSection_LinearColor(
			0,
			Section.Vertices,
			Section.Triangles,
			Section.Normals,
			Section.UVs,
			{},           // Vertex Colors (unused)
			ProcTangents, // Tangents
			true          // Enable collision
		);

		// --- Assign material ---
		Mesh->SetMaterial(
			0,
			Section.Material ? Section.Material :
			LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial"))
		);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}


	// Finish spawning to ensure full initialization
	UGameplayStatics::FinishSpawningActor(NodeActor, NodeActor->GetTransform());

	// Recursively spawn children
	for (const FModelNodeData& Child : Node.Children)
	{
		SpawnNodeRecursive(World, Child, NodeActor, Lods, DamagedName, DestroyedName, BattlespaceName);
	}
}

FTransform UAssimpRuntime3DModelsImporter::ConvertAssimpMatrix(const aiMatrix4x4& AssimpMatrix)
{
	aiVector3D Scale, Position;
	aiQuaternion Rotation;
	AssimpMatrix.Decompose(Scale, Rotation, Position);

	FVector UE_Position = FVector(Position.x, Position.y, Position.z);
	FQuat UE_Rotation = FQuat(Rotation.x, Rotation.y, Rotation.z, Rotation.w);
	FVector UE_Scale = FVector(Scale.x, Scale.y, Scale.z);

	// Standard conversion (GLTF/GLB specific correction)
	UE_Position = FVector(Position.x, Position.z, Position.y);
	UE_Rotation = FQuat(Rotation.x, Rotation.z, Rotation.y, -Rotation.w);
	UE_Scale = FVector(Scale.x, Scale.z, Scale.y);

	return FTransform(UE_Rotation, UE_Position, UE_Scale);
}

void UAssimpRuntime3DModelsImporter::LoadMasterMaterial()
{
	MasterMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/RuntimeModelImporter/M_BaseMaterial.M_BaseMaterial"));

	if (!MasterMaterial)
	{
		MasterMaterial = LoadObject<UMaterial>(nullptr, TEXT("/RuntimeModelsImporter/Materials/RuntimeModelImporter/M_BaseMaterial.M_BaseMaterial"));
	}

	if (!MasterMaterial)
	{
		MasterMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		UE_LOG(LogTemp, Warning, TEXT("⚠️ No custom M_BaseMaterial found. Using Engine fallback."));
	}
}

UMaterialInstanceDynamic* UAssimpRuntime3DModelsImporter::CreateMaterialFromAssimp(
	aiMaterial* AssimpMaterial,
	const aiScene* Scene,
	const FString& FbxFilePath)
{
	if (!AssimpMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Null AssimpMaterial provided."));
		return nullptr;
	}

	// Material name
	aiString AssimpMatName;
	if (AssimpMaterial->Get(AI_MATKEY_NAME, AssimpMatName) != AI_SUCCESS)
		AssimpMatName = aiString("UnnamedMaterial");
	FString MaterialNameStr = UTF8_TO_TCHAR(AssimpMatName.C_Str());

	// ----------------------------
	// Cache check first!
	// ----------------------------
	if (MaterialCache.Contains(AssimpMaterial))
	{
		UE_LOG(LogTemp, Log, TEXT("🔹 Using cached Material Instance: %s"), *MaterialNameStr);
		return MaterialCache[AssimpMaterial];
	}

	// Only log creation if new
	UE_LOG(LogTemp, Log, TEXT("🔹 Creating Material Instance: %s"), *MaterialNameStr);

	if (!MasterMaterial)
		LoadMasterMaterial();
	if (!MasterMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No valid MasterMaterial loaded."));
		return nullptr;
	}

	// Create dynamic instance
	UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(MasterMaterial, GetTransientPackage());
	if (!MatInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to create dynamic material instance."));
		return nullptr;
	}

	const FString BaseDir = FPaths::GetPath(FbxFilePath);

	LoadedMaterials.Add(MatInstance);
	MaterialCache.Add(AssimpMaterial, MatInstance);

	// ----------------------------
	// Track applied parameters to avoid duplicates
	// ----------------------------

	// Fallback BaseColor
	aiColor3D DiffuseColor(0.f, 0.f, 0.f);
	if (AssimpMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS)
	{
		FLinearColor Color(DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		MatInstance->SetVectorParameterValue("BaseColor", Color);
		UE_LOG(LogTemp, Log, TEXT("   [%s] 🎨 Fallback Base Color: R=%.2f G=%.2f B=%.2f"),
			*MaterialNameStr, Color.R, Color.G, Color.B);
	}

	TSet<FName> AppliedParameters;
	// ----------------------------
	// Lambda to apply textures
	// ----------------------------
	auto TryApplyTexture = [&](aiTextureType Type, const FName& ParamName, bool bIsColor)
		{
			/*if (AppliedParameters.Contains(ParamName))
				return;*/

			aiString TexPath;
			if (AssimpMaterial->GetTexture(Type, 0, &TexPath) == AI_SUCCESS)
			{
				FString Path = UTF8_TO_TCHAR(TexPath.C_Str());
				UTexture2D* Texture = nullptr;
				
				FString AppliedTextureName = "None";

				if (!TextureCache.Contains(TexPath.C_Str()))
				{


					// Check if this is DDS
					bool bIsDDS = FPaths::GetExtension(Path).Equals(TEXT("dds"), ESearchCase::IgnoreCase);

					if (!bIsDDS)
					{
						// Use embedded for non-DDS (PNG/JPG/TGA/BMP)
						const aiTexture* Embedded = Scene->GetEmbeddedTexture(TexPath.C_Str());
						if (Embedded)
						{
							Texture = CreateTextureFromEmbedded(Embedded, Path, Type, MaterialNameStr, ParamName);
							AppliedTextureName = FString::Printf(TEXT("Embedded (%s)"), *Path);
						}
					}

					// Load external file for DDS or if embedded not found
					if (!Texture)
					{
						FString FileNameOnly = FPaths::GetCleanFilename(Path);
						TArray<FString> FoundFiles;
						IFileManager::Get().FindFilesRecursive(FoundFiles, *BaseDir, *FileNameOnly, true, false);

						FString FoundTexturePath = (FoundFiles.Num() > 0) ? FoundFiles[0] : FPaths::Combine(BaseDir, Path);
						//FPaths::NormalizeFilename(FoundTexturePath);

						if (FPaths::FileExists(FoundTexturePath))
						{
							Texture = LoadTextureFromDisk(FoundTexturePath, MaterialNameStr, ParamName, Type);
							AppliedTextureName = FString::Printf(TEXT("%s File (%s)"), bIsDDS ? TEXT("DDS") : TEXT("External"), *FoundTexturePath);
						}
					}
				}
				else
				{
					Texture = TextureCache[TexPath.C_Str()];

				}

				// Apply texture if loaded
				if (Texture)
				{
					
					//Texture->NeverStream = false;
					//Texture->VirtualTextureStreaming = true;
					//Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
					//Texture->MipGenSettings = TMGS_SimpleAverage;
					//Texture->MaxTextureSize = 1024;
					//Texture->Modify();
					//Texture->UpdateResource();
					Texture->SRGB = bIsColor;
					if (Type == aiTextureType_OPACITY)
					{
						UMaterial* _material = LoadObject<UMaterial>(nullptr, TEXT("/Game/Materials/RuntimeModelImporter/M_BaseMaterialTransparent.M_BaseMaterialTransparent"));
						UMaterialInstanceDynamic* MatIns = UMaterialInstanceDynamic::Create(_material, GetTransientPackage());
						MatInstance = MatIns;
					}
					else
					{
					MatInstance->SetTextureParameterValue(ParamName, Texture);
					AppliedParameters.Add(ParamName);

					}
					UE_LOG(LogTemp, Display, TEXT("   [%s] ✅ Texture applied: %s -> Parameter: %s"),
						*MaterialNameStr, *AppliedTextureName, *ParamName.ToString());
					if (!TextureCache.Contains(TexPath.C_Str()))
						TextureCache.Add(TexPath.C_Str(), Texture);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("   [%s] ⚠️ Texture not found for Parameter: %s"),
						*MaterialNameStr, *ParamName.ToString());
				}
			}
		};



	/*Async(EAsyncExecution::Thread, [this, TryApplyTexture, AssimpMaterial, Scene, MaterialNameStr, BaseDir]()
		{*/
		// ----------------------------
		// Apply all 6 parameters
		// ----------------------------

	TryApplyTexture(aiTextureType_DIFFUSE, "BaseColor", true);
	TryApplyTexture(aiTextureType_BASE_COLOR, "BaseColor", true);

	TryApplyTexture(aiTextureType_NORMALS, "Normal", false);
	TryApplyTexture(aiTextureType_NORMAL_CAMERA, "Normal", false);
	TryApplyTexture(aiTextureType_HEIGHT, "Normal", false);
	TryApplyTexture(aiTextureType_DISPLACEMENT, "Normal", false);

	TryApplyTexture(aiTextureType_METALNESS, "Metallic", false);
	// TryApplyTexture(aiTextureType_SPECULAR, "Metallic", false);

	TryApplyTexture(aiTextureType_SPECULAR, "Specular", false);

	TryApplyTexture(aiTextureType_AMBIENT_OCCLUSION, "AmbientOcclusion", false);
	//TryApplyTexture(aiTextureType_AMBIENT, "AmbientOcclusion", false);

	TryApplyTexture(aiTextureType_DIFFUSE_ROUGHNESS, "Roughness", false);
	// TryApplyTexture(aiTextureType_SHININESS, "Roughness", false);

	TryApplyTexture(aiTextureType_EMISSION_COLOR, "Emmisive", false);
	//TryApplyTexture(aiTextureType_EMISSIVE, "Emmisive", false);

	TryApplyTexture(aiTextureType_OPACITY, "Opacity", false);

	//	}
	//);
	UE_LOG(LogTemp, Log, TEXT("🔹 Finished Material Instance: %s"), *MaterialNameStr);

	return MatInstance;
}


UTexture2D* UAssimpRuntime3DModelsImporter::CreateTextureFromEmbedded(
	const aiTexture* EmbeddedTex,
	const FString& DebugName,
	aiTextureType Type,
	const FString& MaterialName,
	const FName& ParamName)
{
	if (!EmbeddedTex) return nullptr;

	UTexture2D* Texture = nullptr;

	// --- Compressed texture (PNG/JPG) ---
	if (EmbeddedTex->mHeight == 0)
	{
		const uint8* CompressedData = reinterpret_cast<const uint8*>(EmbeddedTex->pcData);
		int32 DataSize = EmbeddedTex->mWidth;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
		EImageFormat Format = ImageWrapperModule.DetectImageFormat(CompressedData, DataSize);
		TSharedPtr<IImageWrapper> Wrapper = ImageWrapperModule.CreateImageWrapper(Format);

		if (!Wrapper.IsValid() || !Wrapper->SetCompressed(CompressedData, DataSize)) return nullptr;

		TArray64<uint8> RawData;
		if (!Wrapper->GetRaw(ERGBFormat::BGRA, 8, RawData)) return nullptr;

		int32 Width = Wrapper->GetWidth();
		int32 Height = Wrapper->GetHeight();

		Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
		if (!Texture) return nullptr;
#if WITH_EDITORONLY_DATA
		Texture->MipGenSettings = TMGS_FromTextureGroup;
#endif
		Texture->NeverStream = true;

		// --- SRGB / TextureGroup ---
		if (Type == aiTextureType_DIFFUSE || Type == aiTextureType_BASE_COLOR || Type == aiTextureType_EMISSIVE)
		{
			Texture->SRGB = true;
			Texture->LODGroup = TEXTUREGROUP_World;
		}
		else if (Type == aiTextureType_NORMALS || Type == aiTextureType_HEIGHT)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_WorldNormalMap;
		}
		else if (Type == aiTextureType_METALNESS || Type == aiTextureType_DIFFUSE_ROUGHNESS)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_WorldSpecular;
		}
		else if (Type == aiTextureType_AMBIENT_OCCLUSION || Type == aiTextureType_SPECULAR)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_World;
		}
		else
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_World;
		}

		// --- Copy raw data ---
		FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
		void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
		Mip.BulkData.Unlock();

		Texture->UpdateResource();
		Texture->SetFlags(RF_Transient);

		UE_LOG(LogTemp, Display, TEXT("[%s] ✅ Loaded embedded texture for parameter %s: %s (%dx%d)"),
			*MaterialName, *ParamName.ToString(), *DebugName, Width, Height);
	}
	else // --- Raw RGBA texture ---
	{
		int32 Width = EmbeddedTex->mWidth;
		int32 Height = EmbeddedTex->mHeight;

		Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
#if WITH_EDITORONLY_DATA
		Texture->MipGenSettings = TMGS_FromTextureGroup;
#endif
		Texture->NeverStream = true;

		// SRGB / TextureGroup
		if (Type == aiTextureType_DIFFUSE || Type == aiTextureType_BASE_COLOR || Type == aiTextureType_EMISSIVE)
		{
			Texture->SRGB = true;
			Texture->LODGroup = TEXTUREGROUP_World;
		}
		else if (Type == aiTextureType_NORMALS || Type == aiTextureType_HEIGHT)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_WorldNormalMap;
		}
		else if (Type == aiTextureType_METALNESS || Type == aiTextureType_DIFFUSE_ROUGHNESS)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_WorldSpecular;
		}
		else if (Type == aiTextureType_AMBIENT_OCCLUSION || Type == aiTextureType_SPECULAR)
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_World;
		}
		else
		{
			Texture->SRGB = false;
			Texture->LODGroup = TEXTUREGROUP_World;
		}

		void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, EmbeddedTex->pcData, Width * Height * sizeof(FColor));
		Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

		Texture->UpdateResource();
		Texture->SetFlags(RF_Transient);

		UE_LOG(LogTemp, Display, TEXT("[%s] ✅ Loaded raw embedded texture for parameter %s: %s (%dx%d)"),
			*MaterialName, *ParamName.ToString(), *DebugName, Width, Height);
	}

	return Texture;
}



UTexture2D* UAssimpRuntime3DModelsImporter::LoadTextureFromDisk(
	const FString& TexturePath,
	const FString& MaterialName,
	const FName& ParamName,
	aiTextureType Type)
{
	if (!FPaths::FileExists(TexturePath)) return nullptr;

	FString Extension = FPaths::GetExtension(TexturePath).ToLower();

	if (Extension == "dds")
	{
		return LoadDDSTexture(TexturePath, Type); // Keep existing DDS loader
	}

	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *TexturePath)) return nullptr;

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
	EImageFormat Format = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());
	if (Format == EImageFormat::Invalid) return nullptr;

	TSharedPtr<IImageWrapper> Wrapper = ImageWrapperModule.CreateImageWrapper(Format);
	if (!Wrapper.IsValid() || !Wrapper->SetCompressed(FileData.GetData(), FileData.Num())) return nullptr;

	TArray64<uint8> RawData;
	if (!Wrapper->GetRaw(ERGBFormat::BGRA, 8, RawData)) return nullptr;

	int32 Width = Wrapper->GetWidth();
	int32 Height = Wrapper->GetHeight();

	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TMGS_FromTextureGroup;
#endif
	Texture->NeverStream = true;

	// SRGB / TextureGroup
	if (Type == aiTextureType_DIFFUSE || Type == aiTextureType_BASE_COLOR || Type == aiTextureType_EMISSIVE)
	{
		Texture->SRGB = true;
		Texture->LODGroup = TEXTUREGROUP_World;
	}
	else if (Type == aiTextureType_NORMALS || Type == aiTextureType_HEIGHT)
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_WorldNormalMap;
	}
	else if (Type == aiTextureType_METALNESS || Type == aiTextureType_DIFFUSE_ROUGHNESS || Type == aiTextureType_SPECULAR)
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_WorldSpecular;
	}
	else if (Type == aiTextureType_AMBIENT_OCCLUSION )
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_World;
	}
	else
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_World;
	}

	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* TextureData = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
	Mip.BulkData.Unlock();

	Texture->UpdateResource();
	Texture->SetFlags(RF_Transient);

	UE_LOG(LogTemp, Display, TEXT("[%s] ✅ Loaded texture for parameter %s: %s (%dx%d)"),
		*MaterialName, *ParamName.ToString(), *TexturePath, Width, Height);

	return Texture;
}


UTexture2D* UAssimpRuntime3DModelsImporter::LoadDDSTexture(
	const FString& DDSTexture,
	aiTextureType Type // New: pass texture type to handle SRGB correctly
)
{
	if (!FPaths::FileExists(DDSTexture))
	{
		UE_LOG(LogTemp, Error, TEXT("❌ DDS file not found: %s"), *DDSTexture);
		return nullptr;
	}

	// Load DDS using DirectXTex
	DirectX::ScratchImage ScratchImage;
	std::wstring WPath = *DDSTexture;
	HRESULT Hr = DirectX::LoadFromDDSFile(WPath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, ScratchImage);
	if (FAILED(Hr))
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to load DDS file: %s"), *DDSTexture);
		return nullptr;
	}

	const DirectX::TexMetadata& Meta = ScratchImage.GetMetadata();
	const DXGI_FORMAT TargetFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	DirectX::ScratchImage FinalImage;
	const bool bNeedsDecompress = DirectX::IsCompressed(Meta.format);
	const bool bNeedsConvert = Meta.format != TargetFormat;

	if (bNeedsDecompress)
	{
		Hr = DirectX::Decompress(ScratchImage.GetImages(), ScratchImage.GetImageCount(), Meta, TargetFormat, FinalImage);
	}
	else if (bNeedsConvert)
	{
		Hr = DirectX::Convert(ScratchImage.GetImages(), ScratchImage.GetImageCount(), Meta, TargetFormat, DirectX::TEX_FILTER_DEFAULT, 0.f, FinalImage);
	}
	else
	{
		FinalImage = std::move(ScratchImage);
		Hr = S_OK;
	}

	if (FAILED(Hr))
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to convert DDS to target format: %s"), *DDSTexture);
		return nullptr;
	}

	const DirectX::TexMetadata& FinalMeta = FinalImage.GetMetadata();
	const uint32 Width = static_cast<uint32>(FinalMeta.width);
	const uint32 Height = static_cast<uint32>(FinalMeta.height);
	const uint32 MipLevels = static_cast<uint32>(FinalMeta.mipLevels);

	// Create Unreal texture
	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	if (!Texture)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to create texture: %s"), *DDSTexture);
		return nullptr;
	}

	

	//FTexturePlatformData* PlatformData = Texture->GetPlatformData();
	Texture->GetPlatformData()->Mips.Empty();
	Texture->GetPlatformData()->SizeX = Width;
	Texture->GetPlatformData()->SizeY = Height;
	Texture->GetPlatformData()->PixelFormat = PF_B8G8R8A8;

	// Copy all mip levels
	for (uint32 MipIndex = 0; MipIndex < MipLevels; ++MipIndex)
	{
		const DirectX::Image* MipImage = FinalImage.GetImage(MipIndex, 0, 0);
		if (!MipImage || !MipImage->pixels)
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠️ Skipping invalid mip level %u for texture %s"), MipIndex, *DDSTexture);
			continue;
		}

		FTexture2DMipMap* Mip = new FTexture2DMipMap();
		Mip->SizeX = MipImage->width;
		Mip->SizeY = MipImage->height;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		void* Dest = Mip->BulkData.Realloc(MipImage->slicePitch);
		FMemory::Memcpy(Dest, MipImage->pixels, MipImage->slicePitch);
		Mip->BulkData.Unlock();

		Texture->GetPlatformData()->Mips.Add(Mip);
		
		Dest = nullptr;
		delete Dest;
		Mip = nullptr;
		delete Mip;

	}

	// --- Set SRGB / TextureGroup based on type ---
	if (Type == aiTextureType_DIFFUSE || Type == aiTextureType_BASE_COLOR || Type == aiTextureType_EMISSIVE)
	{
		Texture->SRGB = true;
		Texture->LODGroup = TEXTUREGROUP_World;
	}
	else if (Type == aiTextureType_NORMALS || Type == aiTextureType_HEIGHT)
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_WorldNormalMap;
	}
	else if (Type == aiTextureType_METALNESS || Type == aiTextureType_DIFFUSE_ROUGHNESS)
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_WorldSpecular;
	}
	else if (Type == aiTextureType_AMBIENT_OCCLUSION || Type == aiTextureType_SPECULAR)
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_World;
	}
	else
	{
		Texture->SRGB = false;
		Texture->LODGroup = TEXTUREGROUP_World;
	}

#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TMGS_NoMipmaps; // Already using loaded mips
#endif
	Texture->NeverStream = true;
	Texture->CompressionSettings = TC_Default;
	Texture->SRGB = true;
	Texture->UpdateResource();
	Texture->MarkPackageDirty();
	//Texture->PostEditChange();
	Texture->Modify();
	//Texture->AddToRoot(); // prevent GC

	UE_LOG(LogTemp, Display, TEXT("✅ DDS loaded: %s (%dx%d, Mips=%d, Type=%d)"), *DDSTexture, Width, Height, MipLevels, (int32)Type);

	return Texture;
}


bool UAssimpRuntime3DModelsImporter::IsVectorFinite(const FVector& Vec)
{
	return FMath::IsFinite(Vec.X) && FMath::IsFinite(Vec.Y) && FMath::IsFinite(Vec.Z);
}

bool UAssimpRuntime3DModelsImporter::IsTransformValid(const FTransform& Transform)
{
	return IsVectorFinite(Transform.GetLocation()) && IsVectorFinite(Transform.GetScale3D());
}

void UAssimpRuntime3DModelsImporter::LoadAssimpDLLIfNeeded()
{
	static bool bLoaded = false;
	if (bLoaded) return;

	FString PluginDir = FPaths::ProjectPluginsDir();  // Handles both Editor & Packaged
	FString DllPath = FPaths::Combine(PluginDir, TEXT("RuntimeModelsImporter/Binaries/Win64/assimp-vc143-mt.dll"));

	if (FPaths::FileExists(DllPath))
	{
		void* Handle = FPlatformProcess::GetDllHandle(*DllPath);
		if (Handle)
		{
			bLoaded = true;
			UE_LOG(LogTemp, Display, TEXT("✅ Assimp DLL loaded successfully from: %s"), *DllPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Failed to load Assimp DLL from: %s"), *DllPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Assimp DLL not found at: %s"), *DllPath);
	}
}

AActor* UAssimpRuntime3DModelsImporter::GetNodeActorByName(const FString& NodeName) const
{
	if (SpawnedNodeActors.Contains(NodeName))
	{
		return SpawnedNodeActors[NodeName];
	}
	return nullptr;
}

const FModelNodeData* UAssimpRuntime3DModelsImporter::FindNodeDataByName(const FString& NodeName) const
{
	std::function<const FModelNodeData* (const FModelNodeData&)> RecursiveSearch;
	RecursiveSearch = [&](const FModelNodeData& Node) -> const FModelNodeData*
		{
			if (Node.Name == NodeName)
				return &Node;

			for (const FModelNodeData& Child : Node.Children)
			{
				if (const FModelNodeData* Found = RecursiveSearch(Child))
					return Found;
			}
			return nullptr;
		};

	return RecursiveSearch(RootNode);
}


FString UAssimpRuntime3DModelsImporter::GetFilePath() const
{
	return FilePath;
}

void UAssimpRuntime3DModelsImporter::ImportModel(const FString& InFilePath)
{
	FilePath = InFilePath;
	ModelName = FPaths::GetBaseFilename(FilePath);

	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(TCHAR_TO_UTF8(*FilePath),
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_OptimizeMeshes |
		aiProcess_FlipUVs);

	if (!Scene || !Scene->mRootNode)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to load FBX: %s"), *FilePath);
		return;
	}
	DebugAllTexturesInScene(Scene, FilePath);
	RootNode = FModelNodeData();
	ParseNode(Scene->mRootNode, Scene, RootNode, FilePath);
}

void UAssimpRuntime3DModelsImporter::HideModel()
{
	if (RootFBXActor)
	{
		RootFBXActor->SetActorHiddenInGame(true);
		RootFBXActor->SetActorEnableCollision(false);
		RootFBXActor->SetActorTickEnabled(false);
	}

	for (const auto& Pair : SpawnedNodeActors)
	{
		AActor* NodeActor = Pair.Value;
		if (!NodeActor) continue;

		NodeActor->SetActorHiddenInGame(true);
		NodeActor->SetActorEnableCollision(false);
		NodeActor->SetActorTickEnabled(false);

		TArray<UActorComponent*> Comps;
		NodeActor->GetComponents(Comps);
		for (UActorComponent* Comp : Comps)
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
			{
				Prim->SetVisibility(false, true);
				Prim->SetHiddenInGame(true);
			}
		}
	}
}

void UAssimpRuntime3DModelsImporter::ApplyTransform(const FTransform& modelTransform)
{
	if (RootFBXActor)
	{
		RootFBXActor->SetActorTransform(modelTransform);
	}
}



void UAssimpRuntime3DModelsImporter::DebugAllTexturesInScene(const aiScene* Scene, const FString& InFilePath)
{
	if (!Scene)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ No scene to debug textures"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("🔍 DEBUGGING ALL TEXTURES IN FBX FILE: %s"), *InFilePath);
	UE_LOG(LogTemp, Warning, TEXT("======================================================"));

	// 1. Count total textures in scene
	int32 TotalTextures = Scene->mNumTextures;
	UE_LOG(LogTemp, Warning, TEXT("📊 Total Textures in Scene: %d"), TotalTextures);

	// 2. List all embedded textures
	UE_LOG(LogTemp, Warning, TEXT("📦 EMBEDDED TEXTURES:"));
	for (unsigned int i = 0; i < Scene->mNumTextures; ++i)
	{
		const aiTexture* Texture = Scene->mTextures[i];
		if (!Texture) continue;

		FString TextureName = UTF8_TO_TCHAR(Texture->mFilename.C_Str());
		if (TextureName.IsEmpty())
		{
			TextureName = FString::Printf(TEXT("Embedded_Texture_%d"), i);
		}

		FString Format = (Texture->mHeight == 0) ?
			FString::Printf(TEXT("Compressed, %d bytes"), Texture->mWidth) :
			FString::Printf(TEXT("Raw RGBA, %dx%d pixels"), Texture->mWidth, Texture->mHeight);

		UE_LOG(LogTemp, Warning, TEXT("   [%d] %s - %s"), i, *TextureName, *Format);

		// Additional info for compressed textures
		if (Texture->mHeight == 0)
		{
			// Try to detect format
			const uint8* Data = reinterpret_cast<const uint8*>(Texture->pcData);
			if (Data[0] == 0x89 && Data[1] == 'P' && Data[2] == 'N' && Data[3] == 'G')
			{

				UE_LOG(LogTemp, Warning, TEXT("       Format: PNG"));
			}
			else if (Data[0] == 0xFF && Data[1] == 0xD8 && Data[2] == 0xFF)
			{

				UE_LOG(LogTemp, Warning, TEXT("       Format: JPEG"));
			}
			else if (Data[0] == 'D' && Data[1] == 'D' && Data[2] == 'S' && Data[3] == ' ')
			{

				UE_LOG(LogTemp, Warning, TEXT("       Format: DDS"));
			}
			else
			{

				UE_LOG(LogTemp, Warning, TEXT("       Format: Unknown"));
			}
		}
	}

	// 3. List all materials and their texture references
	UE_LOG(LogTemp, Warning, TEXT("🎨 MATERIAL TEXTURE REFERENCES:"));
	for (unsigned int i = 0; i < Scene->mNumMaterials; ++i)
	{
		aiMaterial* Material = Scene->mMaterials[i];
		if (!Material) continue;

		aiString MatName;
		Material->Get(AI_MATKEY_NAME, MatName);
		FString MaterialName = UTF8_TO_TCHAR(MatName.C_Str());
		if (MaterialName.IsEmpty()) MaterialName = FString::Printf(TEXT("Unnamed_Material_%d"), i);

		UE_LOG(LogTemp, Warning, TEXT("   Material %d: %s"), i, *MaterialName);

		// Check all texture types
		for (int32 TextureType = aiTextureType_NONE; TextureType < aiTextureType_UNKNOWN; ++TextureType)
		{
			aiTextureType Type = static_cast<aiTextureType>(TextureType);
			unsigned int TextureCount = Material->GetTextureCount(Type);

			if (TextureCount > 0)
			{
				FString TypeName = GetTextureTypeName(Type);
				UE_LOG(LogTemp, Warning, TEXT("      %s: %d texture(s)"), *TypeName, TextureCount);

				for (unsigned int j = 0; j < TextureCount; ++j)
				{
					aiString TexturePath;
					aiTextureMapping Mapping;
					unsigned int UVIndex;
					float BlendFactor;
					aiTextureOp Operation;
					aiTextureMapMode MapMode;

					if (Material->GetTexture(Type, j, &TexturePath, &Mapping, &UVIndex, &BlendFactor, &Operation, &MapMode) == AI_SUCCESS)
					{
						FString Path = UTF8_TO_TCHAR(TexturePath.C_Str());
						UE_LOG(LogTemp, Warning, TEXT("         [%d] %s"), j, *Path);
						UE_LOG(LogTemp, Warning, TEXT("            Mapping: %d, UV Index: %d, Blend: %.2f"),
							Mapping, UVIndex, BlendFactor);
					}
				}
			}
		}
	}

	// 4. Check for external texture files that might be referenced
	UE_LOG(LogTemp, Warning, TEXT("📁 POTENTIAL EXTERNAL TEXTURE REFERENCES:"));
	TSet<FString> UniqueExternalTextures;

	// Get base directory of FBX file
	FString BaseDir = FPaths::GetPath(InFilePath);

	// Look through all materials for external texture references
	for (unsigned int i = 0; i < Scene->mNumMaterials; ++i)
	{
		aiMaterial* Material = Scene->mMaterials[i];
		if (!Material) continue;

		for (int32 TextureType = aiTextureType_NONE; TextureType < aiTextureType_UNKNOWN; ++TextureType)
		{
			aiTextureType Type = static_cast<aiTextureType>(TextureType);
			unsigned int TextureCount = Material->GetTextureCount(Type);

			for (unsigned int j = 0; j < TextureCount; ++j)
			{
				aiString TexturePath;
				if (Material->GetTexture(Type, j, &TexturePath) == AI_SUCCESS)
				{
					FString Path = UTF8_TO_TCHAR(TexturePath.C_Str());

					// Check if this is likely an external file (not embedded)
					if (!Path.IsEmpty() && !Path.StartsWith("*"))
					{
						UniqueExternalTextures.Add(Path);
					}
				}
			}
		}
	}

	// List unique external texture references
	for (const FString& TexturePath : UniqueExternalTextures)
	{
		FString FullPath = FPaths::Combine(BaseDir, TexturePath);
		bool bExists = FPaths::FileExists(FullPath);

		UE_LOG(LogTemp, Warning, TEXT("   %s -> %s"),
			*TexturePath,
			bExists ? TEXT("✅ EXISTS") : TEXT("❌ MISSING"));
	}

	UE_LOG(LogTemp, Warning, TEXT("======================================================"));
	UE_LOG(LogTemp, Warning, TEXT("📋 SUMMARY:"));
	UE_LOG(LogTemp, Warning, TEXT("   Embedded Textures: %d"), Scene->mNumTextures);
	UE_LOG(LogTemp, Warning, TEXT("   External References: %d"), UniqueExternalTextures.Num());
	UE_LOG(LogTemp, Warning, TEXT("   Total Materials: %d"), Scene->mNumMaterials);
}

FString UAssimpRuntime3DModelsImporter::GetTextureTypeName(aiTextureType Type)
{
	switch (Type)
	{
	case aiTextureType_NONE: return TEXT("NONE");
	case aiTextureType_DIFFUSE: return TEXT("DIFFUSE");
	case aiTextureType_SPECULAR: return TEXT("SPECULAR");
	case aiTextureType_AMBIENT: return TEXT("AMBIENT");
	case aiTextureType_EMISSIVE: return TEXT("EMISSIVE");
	case aiTextureType_HEIGHT: return TEXT("HEIGHT");
	case aiTextureType_NORMALS: return TEXT("NORMALS");
	case aiTextureType_SHININESS: return TEXT("SHININESS");
	case aiTextureType_OPACITY: return TEXT("OPACITY");
	case aiTextureType_DISPLACEMENT: return TEXT("DISPLACEMENT");
	case aiTextureType_LIGHTMAP: return TEXT("LIGHTMAP");
	case aiTextureType_REFLECTION: return TEXT("REFLECTION");
	case aiTextureType_BASE_COLOR: return TEXT("BASE_COLOR");
	case aiTextureType_NORMAL_CAMERA: return TEXT("NORMAL_CAMERA");
	case aiTextureType_EMISSION_COLOR: return TEXT("EMISSION_COLOR");
	case aiTextureType_METALNESS: return TEXT("METALNESS");
	case aiTextureType_DIFFUSE_ROUGHNESS: return TEXT("DIFFUSE_ROUGHNESS");
	case aiTextureType_AMBIENT_OCCLUSION: return TEXT("AMBIENT_OCCLUSION");
	default: return FString::Printf(TEXT("UNKNOWN (%d)"), Type);
	}
}