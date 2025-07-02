#include "WellShot_Spawn_Location.h"

//-------------------------------------------------------------------------------------------------------------
AWellShot_Spawn_Location::AWellShot_Spawn_Location()
{
   PrimaryActorTick.bCanEverTick = false;

   // �������� �� ���������
   Player_Start_Count = 3;
   Spawn_Radius = 200.0f;
   Offset_Z = 100.0f;

   RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
   RootComponent->SetMobility(EComponentMobility::Movable);
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Spawn_Location::OnConstruction(const FTransform& transform)
{
   Super::OnConstruction(transform);

   // ��������� ������ ��� ��������� ����������
   Update_Player_Starts();
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Spawn_Location::BeginPlay()
{
   Super::BeginPlay();

   // ��������� ������ ��� ������ ����
   Update_Player_Starts();
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Spawn_Location::Update_Player_Starts()
{
   // ������� ������ ����������
   for (UChildActorComponent* component : Player_Start_Components)
   {
      if (IsValid(component))
      {
         component->DestroyComponent();
      }
   }
   Player_Start_Components.Empty();

   // ������������ PlayerStart �� ��������� �����
   float angle_step = 360.0f / Player_Start_Count; // ��� ���� ����� �������

   for (int32 i = 0; i < Player_Start_Count; ++i)
   {
      // ��������� ���� ��� ������� �����
      float angle = FMath::DegreesToRadians(angle_step * i);

      // ��������� ���������� ����� �� ��������� �����
      float x = Spawn_Radius * FMath::Cos(angle);
      float y = Spawn_Radius * FMath::Sin(angle);

      FVector local_offset = FVector(x, y, Offset_Z);

      // ������ ��������� ��� PlayerStart
      UChildActorComponent* new_player_start_component = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
      if (new_player_start_component)
      {
         new_player_start_component->SetupAttachment(RootComponent);
         new_player_start_component->SetChildActorClass(APlayerStart::StaticClass());
         new_player_start_component->SetMobility(EComponentMobility::Movable);
         new_player_start_component->SetRelativeLocation(local_offset);
         new_player_start_component->RegisterComponent();

         // ��������� ���������� PlayerStart
         APlayerStart* spawned_player_start = Cast<APlayerStart>(new_player_start_component->GetChildActor());
         if (spawned_player_start)
         {
            UCapsuleComponent* CollisionCapsule = spawned_player_start->FindComponentByClass<UCapsuleComponent>();
            if (CollisionCapsule)
            {
               CollisionCapsule->SetMobility(EComponentMobility::Movable);
            }
         }

         Player_Start_Components.Add(new_player_start_component);
      }
   }
}
//-------------------------------------------------------------------------------------------------------------
