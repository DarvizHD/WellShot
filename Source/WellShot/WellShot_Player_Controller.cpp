#include "WellShot_Player_Controller.h"

//-------------------------------------------------------------------------------------------------------------
void AWellShot_Player_Controller::BeginPlay()
{
   Super::BeginPlay();   
   
   if (APlayerController* player_controller = Cast<APlayerController>(this))
   {// Проверяем, что PlayerController корректно используется

      if (ULocalPlayer* local_player = player_controller->GetLocalPlayer())
      {// Получаем доступ к локальной системе Enhanced Input Subsystem

         UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(local_player);
         if (subsystem && IMC_Movement)
         {
            subsystem->AddMappingContext(IMC_Movement, 0);
         }
      }
   }
}
//-------------------------------------------------------------------------------------------------------------
