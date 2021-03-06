// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessLogic/CGPawn.h"
#include "ChessLogic/CGChessBoard.h"
#include "ChessLogic/CGTile.h"
#include "ChessLogic/CGPawnMovement.h"
#include "GameLogic/CGChessPlayerController.h"
#include "GameLogic/CGGameState.h"

const FString ACGPawn::PawnFen = TEXT("Pp");

ACGPawn::ACGPawn()
{
	UCGPieceMovementBase* moveComp = CreateDefaultSubobject<UCGPawnMovement>(TEXT("MoveValidator"));
	AddOwnedComponent(moveComp);
}

const bool ACGPawn::IsFenMatches(const TCHAR& iChr) const
{ 
	int idx;
	return PawnFen.FindChar(iChr, idx);
}

const FString ACGPawn::GetFenChar() const
{
	return IsWhite() ? TEXT("P") : TEXT("p");
}

void ACGPawn::MoveToTileInternal(ACGTile* iTile, FCGUndo& oUndo, bool iEvents)
{
	//en passant capture
	if (EnPassantTile && EnPassantTile == iTile)
	{
		ACGTile* otherPawnTile = EnPassantTile->Neighbours[(IsBlack() ? EDir::NORTH : EDir::SOUTH)];
		if (ACGPawn* otherPawn = Cast<ACGPawn>(otherPawnTile->OccupiedBy))
		{
			otherPawn->Flags |= EPieceFlags::EnPassantCaptured;
			oUndo.Capture = otherPawn;
			otherPawn->Capture(iEvents);
		}
	}
	//en passant restore
	if ((Flags & EPieceFlags::EnPassantCaptured) == EPieceFlags::EnPassantCaptured)
	{
		//fix the position of this pawn
		Super::MoveToTileInternal(iTile->Neighbours[IsBlack() ? EDir::SOUTH : EDir::NORTH], oUndo, iEvents);
		Flags = Flags & ~EPieceFlags::EnPassantCaptured;
	}
	else
	{
		Super::MoveToTileInternal(iTile, oUndo, iEvents);
	}
	//promotion
	if (iTile->Position.Y == (IsBlack() ? 0 : Board->Size.Y - 1) && iEvents)
	{
		oUndo.Promotion = this;
		//notify controller
		if (ACGChessPlayerController* pc = GetCGController())
		{
			pc->ClientBeginPromotion();
		}
	}
	oUndo.FiftyMoveCounter = 0;
}

void ACGPawn::FinishPromotion(const FString& iChr, FCGUndo& oUndo)
{
	if (ACGGameState* gameState = GetWorld()->GetGameState<ACGGameState>())
	{
		FCGUndo dummyUndo;//we don't want to oUndo the initial piece spawns!
		TSubclassOf<class ACGPiece>* temp = gameState->PieceTemplates.FindByPredicate([&](const TSubclassOf<class ACGPiece>& t) {
			if (const ACGPiece* p = t.Get()->GetDefaultObject<ACGPiece>())
			{
				return (p->IsFenMatches(iChr[0]) && p->IsValidForPromotion());
			}
			return false;
		});
		if (temp)
		{
			Capture();
			//spawn new piece
			FActorSpawnParameters params;
			params.Owner = this;
			ACGPiece* newPiece = GetWorld()->SpawnActor<ACGPiece>(*temp, params);
			newPiece->SetColor(oUndo.Piece->IsWhite());
			newPiece->Board = Board;
			newPiece->MoveToTileInternal(oUndo.To, dummyUndo, false);

			oUndo.Promotion = newPiece;
			newPiece->ClientSnapToPlace();

			Board->Pieces.Add(newPiece);
			//TODO: update the notation
		}
	}
}
