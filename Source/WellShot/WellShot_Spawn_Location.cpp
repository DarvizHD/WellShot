#include "WellShot_Spawn_Location.h"

//-------------------------------------------------------------------------------------------------------------
AWellShot_Spawn_Location::AWellShot_Spawn_Location()
{
   PrimaryActorTick.bCanEverTick = false;

   // Значения по умолчанию
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

   // Обновляем спавны при изменении параметров
   Update_Player_Starts();
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Spawn_Location::BeginPlay()
{
   Super::BeginPlay();

   // Обновляем спавны при старте игры
   Update_Player_Starts();
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Spawn_Location::Update_Player_Starts()
{
   // Удаляем старые компоненты
   for (UChildActorComponent* component : Player_Start_Components)
   {
      if (IsValid(component))
      {
         component->DestroyComponent();
      }
   }
   Player_Start_Components.Empty();

   // Распределяем PlayerStart по периметру круга
   float angle_step = 360.0f / Player_Start_Count; // Шаг угла между точками

   for (int32 i = 0; i < Player_Start_Count; ++i)
   {
      // Вычисляем угол для текущей точки
      float angle = FMath::DegreesToRadians(angle_step * i);

      // Вычисляем координаты точки по периметру круга
      float x = Spawn_Radius * FMath::Cos(angle);
      float y = Spawn_Radius * FMath::Sin(angle);

      FVector local_offset = FVector(x, y, Offset_Z);

      // Создаём компонент для PlayerStart
      UChildActorComponent* new_player_start_component = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass());
      if (new_player_start_component)
      {
         new_player_start_component->SetupAttachment(RootComponent);
         new_player_start_component->SetChildActorClass(APlayerStart::StaticClass());
         new_player_start_component->SetMobility(EComponentMobility::Movable);
         new_player_start_component->SetRelativeLocation(local_offset);
         new_player_start_component->RegisterComponent();

         // Изменение параметров PlayerStart
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
