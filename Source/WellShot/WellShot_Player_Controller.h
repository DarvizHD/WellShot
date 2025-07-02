#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "WellShot_Player_Controller.generated.h"

//-------------------------------------------------------------------------------------------------------------
UCLASS()
class WELLSHOT_API AWellShot_Player_Controller : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputMappingContext* IMC_Movement;

protected:
	virtual void BeginPlay() override;
};
//-------------------------------------------------------------------------------------------------------------
