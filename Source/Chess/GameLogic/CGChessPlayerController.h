// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CGChessPlayerController.generated.h"

class ACGPiece;
class ACGTile;

UCLASS()
class CHESS_API ACGChessPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACGChessPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category="Chess setup")
	int SelectedSkinId{ 0 };

	//0white 1black 2no
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Chess setup")
	int PreferredSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing=SideChanged, Category = "Chess setup")
	bool bIsBlack{ false };
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Chess")
	void ServerMoveToTile(ACGPiece* pPiece, ACGTile* pTile);
	void ServerMoveToTile_Implementation(ACGPiece* pPiece, ACGTile* pTile);

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "Chess")
	void ServerUpdateGrab(ACGPiece* pPiece, FVector_NetQuantize Location);
	void ServerUpdateGrab_Implementation(ACGPiece* pPiece, FVector_NetQuantize pLocation);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Chess")
	void ServerGrab(ACGPiece* pPiece, bool pGrab);
	void ServerGrab_Implementation(ACGPiece* pPiece, bool pGrab);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Chess")
	void ServerUndoTo(int pMoveNum);
	void ServerUndoTo_Implementation(int pMoveNum);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Chess")
	void ServerConcede();
	void ServerConcede_Implementation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Chess")
	void OnWin();

	UFUNCTION(BlueprintImplementableEvent, Category = "Chess")
	void OnLose();

	UFUNCTION(BlueprintImplementableEvent, Category = "Chess")
	void OnDraw();

	UFUNCTION()
	void SideChanged();

	virtual void BeginPlayingState() override;

	//virtual void SetPawn(APawn* InPawn) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
