#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h" // Для репликации
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "WellShot_Character.generated.h"

//-------------------------------------------------------------------------------------------------------------
UCLASS()
class WELLSHOT_API AWellShot_Character : public ACharacter
{
   GENERATED_BODY()

public:
   AWellShot_Character();

   virtual void BeginPlay() override;

   virtual void SetupPlayerInputComponent(class UInputComponent* player_input_component) override;

   void Move(const FInputActionValue& value);
   void Jump_Start(const FInputActionValue& value);
   void Jump_Stop(const FInputActionValue& value);
   void Look(const FInputActionValue& value);
   void Sprint_Start(const FInputActionValue& value);
   void Sprint_Stop(const FInputActionValue& value);
   void Crouch_Start(const FInputActionValue& value);
   void Crouch_Stop(const FInputActionValue& value);

   // --- Переменные для репликации ---
   UPROPERTY(ReplicatedUsing = OnRep_SprintSpeed)
   float SprintSpeed = 400.0f; // Скорость при беге

   UPROPERTY(ReplicatedUsing = OnRep_SprintSpeed)
   float WalkSpeed = 200.0f; // Обычная скорость

   UPROPERTY(ReplicatedUsing = OnRep_SprintSpeed)
   float CrouchSpeed = 100.0f; // Скорость при приседании

   UPROPERTY(ReplicatedUsing = OnRep_MaxWalkSpeed)
   float MaxWalkSpeed = 200.0f;

   UPROPERTY(ReplicatedUsing = OnRep_Crouch)
   bool bIsCrouching = false;

   UPROPERTY(ReplicatedUsing = OnRep_Jump, VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
   bool bIsJumping = false;

   UFUNCTION(BlueprintPure)
   bool GetIsCrouching() const;

   UFUNCTION(BlueprintPure)
   bool GetIsJumping() const;

   // --- RPC методы ---
   UFUNCTION(Server, Reliable, WithValidation)
   void Server_SetSprint(bool bIsSprinting);
   void Server_SetSprint_Implementation(bool bIsSprinting);
   bool Server_SetSprint_Validate(bool bIsSprinting);

   UFUNCTION(Server, Reliable, WithValidation)
   void Server_SetCrouch(bool bCrouch);
   void Server_SetCrouch_Implementation(bool bCrouch);
   bool Server_SetCrouch_Validate(bool bCrouch);

   UFUNCTION(Server, Reliable, WithValidation)
   void Server_SetJump(bool bJumping);
   void Server_SetJump_Implementation(bool bJumping);
   bool Server_SetJump_Validate(bool bJumping);

   // --- RepNotify функции ---
   UFUNCTION()
   void OnRep_SprintSpeed();

   UFUNCTION()
   void OnRep_Crouch();

   UFUNCTION()
   void OnRep_MaxWalkSpeed();

   UFUNCTION()
   void OnRep_Jump();

   // --- Инпуты ---
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputAction* Move_Action;
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputAction* Jump_Action;
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputAction* Look_Action;
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputAction* Sprint_Action;
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input") UInputAction* Crouch_Action;

protected:
   virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
//-------------------------------------------------------------------------------------------------------------