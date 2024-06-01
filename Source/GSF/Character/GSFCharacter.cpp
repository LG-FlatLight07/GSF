#include "GSFCharacter.h"

#include "GSF/Component/GSFInputComp.h"
#include "GSF/Component/GSFMoveComp.h"
#include "GSF/Component/GSFAbilityComp.h"
#include "GSF/Component/GSFAnimComp.h"
#include "GSF/GameMode/GSFGameMode.h"
#include "Kismet/GameplayStatics.h"


class AGSFCamera;

AGSFCharacter::AGSFCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
	
	hitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Box"));
	mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	moveComp = CreateDefaultSubobject<UGSFMoveComp>(TEXT("Move"));
	inputComp = CreateDefaultSubobject<UGSFInputComp>(TEXT("Input"));
	abilityComp = CreateDefaultSubobject<UGSFAbilityComp>(TEXT("Ability"));
	animComp = CreateDefaultSubobject<UGSFAnimComp>(TEXT("Anim"));

	if(hitBox)
		RootComponent = hitBox;
	if(mesh && hitBox)
		mesh->SetupAttachment(hitBox);
}

// Called when the game starts or when spawned
void AGSFCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGSFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGSFCharacter::RecieveDamage_Implementation()
{
}

void AGSFCharacter::Fear_Implementation()
{
}

void AGSFCharacter::Death_Implementation()
{
}

void AGSFCharacter::MoveForward(const float MoveValue)
{
	moveComp->AddForwardMovementInput(MoveValue);
}

void AGSFCharacter::MoveRight(const float MoveValue)
{
	moveComp->AddRightMovementInput(MoveValue);
}

void AGSFCharacter::TakeAction(EInputActions Action)
{
	if(Action == EInputActions::AirDash)
	{
		if(MoveComp()->inputValue.IsZero())
			AirJump();
		else
			AirDash();
	}
	if(Action == EInputActions::Fly)
	{
		Fly();
	}
}

bool AGSFCharacter::IsTakingAction() const
{
	return bTakingAction;
}

void AGSFCharacter::Graid_Implementation(const bool key)
{
	if(key)
	{
		UE_LOG(LogTemp, Log, TEXT("Graid"));
		
		MoveComp()->Graid();
	}
	else
	{
		MoveComp()->GraidEnd();
	}
}

void AGSFCharacter::AirDash_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AirDash"));

	MoveComp()->AirDash();
}

void AGSFCharacter::AirJump_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AirJump"));

	MoveComp()->AirJump();
}

void AGSFCharacter::Fly_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Fly"));
	
	MoveComp()->Fly();
}

void AGSFCharacter::ManualAim_Implementation(const bool look)
{
	AGSFCamera* camera = Cast<AGSFGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->camera;
	if(IsValid(camera))
	{
		camera->SetManualAim(look);
	}
	MoveComp()->SetFloat(look);
}

// Called to bind functionality to input
void AGSFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComp()->SetupInputComp(PlayerInputComponent);
}