#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "WellShot_Weapon.generated.h"

UCLASS()
class WELLSHOT_API AWellShot_Weapon : public AActor
{
   GENERATED_BODY()

public:
   AWellShot_Weapon();
};
