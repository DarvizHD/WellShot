#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerStart.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "WellShot_Spawn_Location.generated.h"

//-------------------------------------------------------------------------------------------------------------
UCLASS()
class WELLSHOT_API AWellShot_Spawn_Location : public AActor
{
   GENERATED_BODY()

public:
   AWellShot_Spawn_Location();

   virtual void OnConstruction(const FTransform& transform) override;

   // Параметры настройки спавна
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") int32 Player_Start_Count;  // Количество точек Player Start
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") float Spawn_Radius;  // Радиус разброса точек спавна
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") float Offset_Z;  // Смещение по высоте для Player Start

protected:
   virtual void BeginPlay() override;

private:
   // Обновление позиций Player Start
   void Update_Player_Starts();

   // Массив компонентов для Player Start
   UPROPERTY() TArray<UChildActorComponent*> Player_Start_Components;
};
//-------------------------------------------------------------------------------------------------------------
