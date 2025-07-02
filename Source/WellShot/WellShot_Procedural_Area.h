#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "WellShot_Procedural_Area.generated.h"

USTRUCT(BlueprintType)
struct FAsset_Spawn_Info
{
   GENERATED_BODY()

   FAsset_Spawn_Info()
   {
      Asset_Class = nullptr;
      Instance_Count = 0;
      Offset_From_Surface = 10.0f;
   }

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Assets")
   TSubclassOf<AActor> Asset_Class;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Procedural Assets", meta = (ClampMin = "1", UIMin = "1"))
   int32 Instance_Count;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
   float Offset_From_Surface;
};

USTRUCT(BlueprintType)
struct FReplicatedAssetData
{
   GENERATED_BODY()
public:

   UPROPERTY()
   FVector Location;

   UPROPERTY()
   FRotator Rotation;

   UPROPERTY()
   TSubclassOf<AActor> AssetClass;
};

UCLASS()
class WELLSHOT_API AWellShot_Procedural_Area : public AActor
{
   GENERATED_BODY()

public:
   AWellShot_Procedural_Area();

   UFUNCTION(BlueprintCallable, Category = "Procedural Landscape")
   void Generate_Landscape();

   UFUNCTION(BlueprintCallable, Category = "Procedural Landscape")
   void Spawn_Random_Assets();

   UFUNCTION(BlueprintCallable, CallInEditor, Category = "Procedural Landscape")
   void Update_Landscape();

   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

   // Landscape parameters
   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape")
   bool Auto_Update_Landscape = true;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape", meta = (ClampMin = "10", UIMin = "10"))
   int32 Width;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape", meta = (ClampMin = "10", UIMin = "10"))
   int32 Length;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape", meta = (ClampMin = "2", UIMin = "2"))
   int32 Vertex_Count_Along_Axis;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Seed, Category = "Procedural Landscape")
   int32 Seed;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape", meta = (ClampMin = "100", UIMin = "100"))
   float Noise_Scale;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape", meta = (ClampMin = "10", UIMin = "10"))
   float Max_Height;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape")
   UMaterialInterface* Landscape_Material;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Landscape")
   float Texture_Tiling = 1.0f;

   // Assets parameters
   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Assets")
   TArray<FAsset_Spawn_Info> Asset_Spawn_Info_Array;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_LandscapeParams, Category = "Procedural Assets", meta = (ClampMin = "10.0", UIMin = "10.0"))
   float Min_Distance_Between_Assets = 50.0f;

protected:
   virtual void BeginPlay() override;
   virtual void OnConstruction(const FTransform& transform) override;

   UFUNCTION()
   void OnRep_LandscapeParams();

   UFUNCTION()
   void OnRep_Seed();

   void Multicast_GenerateLandscape_Implementation();

   UFUNCTION()
   void OnRep_AssetsData();

   UFUNCTION(NetMulticast, Reliable)
   void Multicast_GenerateLandscape();

private:
   void Create_Mesh();
   void Clear_Attached_Actors();
   float Generate_Perlin_Noise(float x, float y) const;

   UPROPERTY(ReplicatedUsing = OnRep_AssetsData)
   TArray<FReplicatedAssetData> ReplicatedAssetsData;

   TArray<int32> Triangles;
   TArray<FVector> Vertices;
   TArray<FVector> Normals;
   TArray<FVector2D> UVs;
   TArray<FColor> Vertex_Colors;
   TArray<FProcMeshTangent> Tangents;

   UPROPERTY()
   TArray<AActor*> Spawned_Assets;

   UPROPERTY(VisibleAnywhere)
   UProceduralMeshComponent* Mesh_Component;
};