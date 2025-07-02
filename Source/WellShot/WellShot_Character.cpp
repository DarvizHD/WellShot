#include "WellShot_Character.h"
#include "WellShot_Weapon.h"
#include "WellShot_Weapon_Shooting.h"

//-------------------------------------------------------------------------------------------------------------
AWellShot_Character::AWellShot_Character()
{
   PrimaryActorTick.bCanEverTick = true;
   bReplicates = true; // Включаем репликацию
   MaxWalkSpeed = WalkSpeed; // Инициализируем MaxWalkSpeed
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::BeginPlay() 
{
   Super::BeginPlay();
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::SetupPlayerInputComponent(UInputComponent* player_input_component)
{
   Super::SetupPlayerInputComponent(player_input_component);

   if (UEnhancedInputComponent* enhanced_input_comp = Cast<UEnhancedInputComponent>(player_input_component))
   {
      if (Move_Action)
         enhanced_input_comp->BindAction(Move_Action, ETriggerEvent::Triggered, this, &AWellShot_Character::Move);

      if (Jump_Action)
      {
         enhanced_input_comp->BindAction(Jump_Action, ETriggerEvent::Triggered, this, &AWellShot_Character::Jump_Start);
         enhanced_input_comp->BindAction(Jump_Action, ETriggerEvent::Completed, this, &AWellShot_Character::Jump_Stop);
      }

      if (Look_Action)
         enhanced_input_comp->BindAction(Look_Action, ETriggerEvent::Triggered, this, &AWellShot_Character::Look);

      if (Sprint_Action)
      {
         enhanced_input_comp->BindAction(Sprint_Action, ETriggerEvent::Started, this, &AWellShot_Character::Sprint_Start);
         enhanced_input_comp->BindAction(Sprint_Action, ETriggerEvent::Completed, this, &AWellShot_Character::Sprint_Stop);
      }

      if (Crouch_Action)
      {
         enhanced_input_comp->BindAction(Crouch_Action, ETriggerEvent::Started, this, &AWellShot_Character::Crouch_Start);
         enhanced_input_comp->BindAction(Crouch_Action, ETriggerEvent::Completed, this, &AWellShot_Character::Crouch_Stop);
      }

   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Move(const FInputActionValue& value)
{
   FVector2D movement_vector = value.Get<FVector2D>();
   AddMovementInput(GetActorForwardVector(), movement_vector.Y);
   AddMovementInput(GetActorRightVector(), movement_vector.X);
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Jump_Start(const FInputActionValue& value)
{
   if (HasAuthority()) // Если это сервер
   {
      bIsJumping = true;
      Jump();
   }
   else // Если это клиент
   {
      Server_SetJump(true);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Jump_Stop(const FInputActionValue& value)
{
   if (HasAuthority()) // Если это сервер
   {
      bIsJumping = false;
      StopJumping();
   }
   else // Если это клиент
   {
      Server_SetJump(false);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Look(const FInputActionValue& value)
{
   FVector2D movement_vector = value.Get<FVector2D>();
   AddControllerYawInput(movement_vector.X);
   AddControllerPitchInput(movement_vector.Y);
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Sprint_Start(const FInputActionValue& value)
{
   if (HasAuthority())
   {
      MaxWalkSpeed = SprintSpeed;
      GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
   }
   else
   {
      Server_SetSprint(true);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Sprint_Stop(const FInputActionValue& value)
{
   if (HasAuthority())
   {
      MaxWalkSpeed = WalkSpeed;
      GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
   }
   else
   {
      Server_SetSprint(false);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Crouch_Start(const FInputActionValue& value)
{
   if (HasAuthority())
   {
      bIsCrouching = true;
      OnRep_Crouch();
   }
   else
   {
      Server_SetCrouch(true);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Crouch_Stop(const FInputActionValue& value)
{
   if (HasAuthority())
   {
      bIsCrouching = false;
      OnRep_Crouch();
   }
   else
   {
      Server_SetCrouch(false);
   }
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::OnRep_SprintSpeed()
{
   MaxWalkSpeed = bIsCrouching ? CrouchSpeed : SprintSpeed;
   GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::OnRep_Crouch()
{
   if (bIsCrouching)
   {
      Crouch();
      MaxWalkSpeed = CrouchSpeed;
   }
   else
   {
      UnCrouch();
      MaxWalkSpeed = WalkSpeed;
   }
   GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::OnRep_MaxWalkSpeed()
{
   GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::OnRep_Jump()
{
   if (bIsJumping)
   {
      Jump();
   }
   else
   {
      StopJumping();
   }
}
//-------------------------------------------------------------------------------------------------------------
bool AWellShot_Character::GetIsCrouching() const
{
   return bIsCrouching;
}
//-------------------------------------------------------------------------------------------------------------
bool AWellShot_Character::GetIsJumping() const
{
   return bIsJumping;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Server_SetSprint_Implementation(bool bIsSprinting)
{
   MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
   GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}
//-------------------------------------------------------------------------------------------------------------
bool AWellShot_Character::Server_SetSprint_Validate(bool bIsSprinting)
{
   return true;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Server_SetCrouch_Implementation(bool bCrouch)
{
   bIsCrouching = bCrouch;
   OnRep_Crouch();
}
//-------------------------------------------------------------------------------------------------------------
bool AWellShot_Character::Server_SetCrouch_Validate(bool bCrouch)
{
   return true;
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::Server_SetJump_Implementation(bool bJumping)
{
   if (bJumping)
   {
      Jump();
   }
   else
   {
      StopJumping();
   }

   bIsJumping = bJumping; // Синхронизируем переменную
}
//-------------------------------------------------------------------------------------------------------------
bool AWellShot_Character::Server_SetJump_Validate(bool bJumping)
{
   return true; // Валидация (можно добавить проверку)
}
//-------------------------------------------------------------------------------------------------------------
void AWellShot_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
   Super::GetLifetimeReplicatedProps(OutLifetimeProps);
   DOREPLIFETIME(AWellShot_Character, SprintSpeed);
   DOREPLIFETIME(AWellShot_Character, WalkSpeed);
   DOREPLIFETIME(AWellShot_Character, CrouchSpeed);
   DOREPLIFETIME(AWellShot_Character, bIsCrouching);
   DOREPLIFETIME(AWellShot_Character, MaxWalkSpeed);
   DOREPLIFETIME(AWellShot_Character, bIsJumping);
}
//-------------------------------------------------------------------------------------------------------------