// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessLogic/CGPawnMovement.h"
#include "ChessLogic/CGPawn.h"
#include "ChessLogic/CGPiece.h"
#include "ChessLogic/CGChessBoard.h"
#include "ChessLogic/CGTile.h"
#include "GameLogic/CGUndo.h"
#include "CGChessBoard.h"


void UCGPawnMovement::GetAvailableMoves(TSet<ACGTile*>& oSet)
{
	ACGPawn* pawn = GetOwner<ACGPawn>();
	if (pawn && pawn->Tile && pawn->Board)
	{
		ACGTile* t = pawn->Tile->Neighbours[pawn->IsBlack() ? EDir::SOUTH : EDir::NORTH];
		if (t && t->OccupiedBy == nullptr)
		{
			oSet.Add(t);
			//can double open?
			if ((pawn->IsBlack() && pawn->Position.Y == pawn->Board->Size.Y - 2) || (pawn->IsWhite() && pawn->Position.Y == 1))
			{
				t = t->Neighbours[pawn->IsBlack() ? EDir::SOUTH : EDir::NORTH];
				if (t && t->OccupiedBy == nullptr)
				{
					oSet.Add(t);
				}
			}
		}
		//attack
		t = pawn->Tile->Neighbours[pawn->IsBlack() ? EDir::SOUTH_EAST : EDir::NORTH_EAST];
		if(t && t->OccupiedBy)
		{
			if (t->OccupiedBy->IsBlack() != pawn->IsBlack())
			{
				oSet.Add(t);
			}
		}
		t = pawn->Tile->Neighbours[pawn->IsBlack() ? EDir::SOUTH_WEST : EDir::NORTH_WEST];
		if (t && t->OccupiedBy)
		{
			if (t->OccupiedBy->IsBlack() != pawn->IsBlack())
			{
				oSet.Add(t);
			}
		}
		//en passant
		if (pawn->Board->Undos.Num() > 0 && (pawn->IsBlack() ? pawn->Position.Y == 3 : pawn->Board->Size.Y - 4 == pawn->Position.Y))//is in the good rank
		{
			for (EDir dir : {EDir::EAST, EDir::WEST})
			{
				t = pawn->Tile->Neighbours[dir];
				if (t && t->OccupiedBy)
				{
					const FCGUndo& undo = pawn->Board->Undos.Last();
					if (t->OccupiedBy->IsA(ACGPawn::StaticClass()) && undo.Piece == t->OccupiedBy)//are we standing next to the last moved pawn
					{
						if (undo.From && undo.To && (abs(undo.From->Position.Y - undo.To->Position.Y) == 2))//was the last move double open?
						{
							pawn->EnPassantTile = t->Neighbours[pawn->IsBlack() ? EDir::SOUTH : EDir::NORTH];
							oSet.Add(pawn->EnPassantTile);
							break;
						}
					}
				}
			}
		}
	}
}

void UCGPawnMovement::GetAttackedTiles(TSet<ACGTile*>& oSet)
{
	ACGPawn* owner = GetOwner<ACGPawn>();
	if (owner)
	{
		ACGTile* t;
		t = owner->Tile->Neighbours[owner->IsBlack() ? EDir::SOUTH_WEST : EDir::NORTH_WEST];
		if (t)
		{
			oSet.Add(t);
		}
		t = owner->Tile->Neighbours[owner->IsBlack() ? EDir::SOUTH_EAST : EDir::NORTH_EAST];
		if (t)
		{
			oSet.Add(t);
		}
	}
	//en passant can't attack kings so it should not matter.
}