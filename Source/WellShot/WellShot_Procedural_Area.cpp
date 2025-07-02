#include "WellShot_Procedural_Area.h"

AWellShot_Procedural_Area::AWellShot_Procedural_Area()
{
   PrimaryActorTick.bCanEverTick = true;
   bReplicates = true;
   bAlwaysRelevant = true;

   Mesh_Component = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
   RootComponent = Mesh_Component;

   Width = 100;
   Length = 100;
   Vertex_Count_Along_Axis = 10;
   Max_Height = 100.0f;
   Seed = FMath::RandRange(0, INT32_MAX);
   Noise_Scale = 50.0f;
}

void AWellShot_Procedural_Area::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);

   DOREPLIFETIME(AWellShot_Procedural_Area, Auto_Update_Landscape);
   DOREPLIFETIME(AWellShot_Procedural_Area, Width);
   DOREPLIFETIME(AWellShot_Procedural_Area, Length);
   DOREPLIFETIME(AWellShot_Procedural_Area, Vertex_Count_Along_Axis);
   DOREPLIFETIME(AWellShot_Procedural_Area, Seed);
   DOREPLIFETIME(AWellShot_Procedural_Area, Noise_Scale);
   DOREPLIFETIME(AWellShot_Procedural_Area, Max_Height);
   DOREPLIFETIME(AWellShot_Procedural_Area, Landscape_Material);
   DOREPLIFETIME(AWellShot_Procedural_Area, Texture_Tiling);
   DOREPLIFETIME(AWellShot_Procedural_Area, Asset_Spawn_Info_Array);
   DOREPLIFETIME(AWellShot_Procedural_Area, Min_Distance_Between_Assets);
   DOREPLIFETIME(AWellShot_Procedural_Area, ReplicatedAssetsData);
}

void AWellShot_Procedural_Area::BeginPlay()
{
   Super::BeginPlay();

   if (HasAuthority())
   {
      Generate_Landscape();
      Spawn_Random_Assets();
   }
}

void AWellShot_Procedural_Area::OnConstruction(const FTransform& transform)
{
   Super::OnConstruction(transform);

   if (Auto_Update_Landscape && HasAuthority())
   {
      Update_Landscape();
   }
}

void AWellShot_Procedural_Area::Generate_Landscape()
{
   Vertices.Empty();
   Triangles.Empty();
   Normals.Empty();
   UVs.Empty();
   Vertex_Colors.Empty();
   Tangents.Empty();

   float step_x = static_cast<float>(Width * 2) / (Vertex_Count_Along_Axis - 1);
   float step_y = static_cast<float>(Length * 2) / (Vertex_Count_Along_Axis - 1);

   for (int32 x = 0; x < Vertex_Count_Along_Axis; x++)
   {
      for (int32 y = 0; y < Vertex_Count_Along_Axis; y++)
      {
         float noise_value = Generate_Perlin_Noise(x * step_x, y * step_y);
         FVector vertex = FVector(x * step_x, y * step_y, noise_value * Max_Height);
         Vertices.Add(vertex);

         if (x < Vertex_Count_Along_Axis - 1 && y < Vertex_Count_Along_Axis - 1)
         {
            Triangles.Add(x + y * Vertex_Count_Along_Axis);
            Triangles.Add((x + 1) + y * Vertex_Count_Along_Axis);
            Triangles.Add((x + 1) + (y + 1) * Vertex_Count_Along_Axis);

            Triangles.Add(x + y * Vertex_Count_Along_Axis);
            Triangles.Add((x + 1) + (y + 1) * Vertex_Count_Along_Axis);
            Triangles.Add(x + (y + 1) * Vertex_Count_Along_Axis);
         }
      }
   }

   for (int32 i = 0; i < Vertices.Num(); i++)
   {
      Normals.Add(FVector(0, 0, 1));
      UVs.Add(FVector2D(Vertices[i].X / Width, Vertices[i].Y / Length) * Texture_Tiling);
   }

   Create_Mesh();

   if (HasAuthority())
   {
      Multicast_GenerateLandscape();
   }
}

void AWellShot_Procedural_Area::Spawn_Random_Assets()
{
   if (!HasAuthority()) return;

   Clear_Attached_Actors();
   ReplicatedAssetsData.Empty();

   if (Asset_Spawn_Info_Array.Num() == 0) return;

   FRandomStream RandomStream(Seed);

   for (const FAsset_Spawn_Info& asset_info : Asset_Spawn_Info_Array)
   {
      if (!asset_info.Asset_Class) continue;

      for (int32 i = 0; i < asset_info.Instance_Count; i++)
      {
         FReplicatedAssetData NewAssetData;
         bool valid_location_found = false;

         for (int32 attempt = 0; attempt < 50; attempt++)
         {
            float random_x = RandomStream.FRandRange(0.0f, Width * 2);
            float random_y = RandomStream.FRandRange(0.0f, Length * 2);
            float z = Generate_Perlin_Noise(random_x, random_y) * Max_Height;

            NewAssetData.Location = FVector(random_x, random_y, z + asset_info.Offset_From_Surface);
            NewAssetData.Rotation = FRotator(0, RandomStream.FRandRange(0.0f, 360.0f), 0);
            NewAssetData.AssetClass = asset_info.Asset_Class;

            valid_location_found = true;
            for (const FReplicatedAssetData& existing_asset : ReplicatedAssetsData)
            {
               if (FVector::Dist(existing_asset.Location, NewAssetData.Location) < Min_Distance_Between_Assets)
               {
                  valid_location_found = false;
                  break;
               }
            }

            if (valid_location_found) break;
         }

         if (valid_location_found)
         {
            ReplicatedAssetsData.Add(NewAssetData);

            // Spawn immediately on server
            AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
               NewAssetData.AssetClass,
               NewAssetData.Location,
               NewAssetData.Rotation
            );

            if (SpawnedActor)
            {
               SpawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
               Spawned_Assets.Add(SpawnedActor);
            }
         }
      }
   }
}

void AWellShot_Procedural_Area::OnRep_AssetsData()
{
   Clear_Attached_Actors();
   Spawned_Assets.Empty();

   for (const FReplicatedAssetData& AssetData : ReplicatedAssetsData)
   {
      AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
         AssetData.AssetClass,
         AssetData.Location,
         AssetData.Rotation
      );

      if (SpawnedActor)
      {
         SpawnedActor->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
         Spawned_Assets.Add(SpawnedActor);
      }
   }
}

void AWellShot_Procedural_Area::OnRep_LandscapeParams()
{
   Generate_Landscape();

   if (HasAuthority())
   {
      Spawn_Random_Assets();
   }
}

void AWellShot_Procedural_Area::OnRep_Seed()
{
   Generate_Landscape();

   if (HasAuthority())
   {
      Spawn_Random_Assets();
   }
}

void AWellShot_Procedural_Area::Multicast_GenerateLandscape_Implementation()
{
   if (!HasAuthority())
   {
      Generate_Landscape();
   }
}

void AWellShot_Procedural_Area::Update_Landscape()
{
   Clear_Attached_Actors();
   Generate_Landscape();
   Spawn_Random_Assets();
}

void AWellShot_Procedural_Area::Create_Mesh()
{
   Mesh_Component->ClearAllMeshSections();
   Mesh_Component->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, Vertex_Colors, Tangents, true);

   if (Landscape_Material)
   {
      Mesh_Component->SetMaterial(0, Landscape_Material);
   }
}

void AWellShot_Procedural_Area::Clear_Attached_Actors()
{
   for (AActor* Actor : Spawned_Assets)
   {
      if (Actor)
      {
         Actor->Destroy();
      }
   }
   Spawned_Assets.Empty();
}

float AWellShot_Procedural_Area::Generate_Perlin_Noise(float x, float y) const
{
   return FMath::PerlinNoise2D(FVector2D(x / Noise_Scale + Seed, y / Noise_Scale + Seed));
}