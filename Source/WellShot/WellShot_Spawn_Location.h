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

   // ��������� ��������� ������
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") int32 Player_Start_Count;  // ���������� ����� Player Start
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") float Spawn_Radius;  // ������ �������� ����� ������
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings") float Offset_Z;  // �������� �� ������ ��� Player Start

protected:
   virtual void BeginPlay() override;

private:
   // ���������� ������� Player Start
   void Update_Player_Starts();

   // ������ ����������� ��� Player Start
   UPROPERTY() TArray<UChildActorComponent*> Player_Start_Components;
};
//-------------------------------------------------------------------------------------------------------------
