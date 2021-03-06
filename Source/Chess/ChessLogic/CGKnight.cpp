// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessLogic/CGKnight.h"
#include "ChessLogic/CGLinearMovement.h"

const FString ACGKnight::KnightFen = TEXT("Nn");

ACGKnight::ACGKnight()
{
	UCGLinearMovement* moveComp = CreateDefaultSubobject<UCGLinearMovement>(TEXT("MoveValidator"));
	moveComp->Directions = { EDir::KNIGHT1, EDir::KNIGHT2, EDir::KNIGHT3, EDir::KNIGHT4, EDir::KNIGHT5, EDir::KNIGHT6, EDir::KNIGHT7, EDir::KNIGHT8 };
	moveComp->Range = 1;
	AddOwnedComponent(moveComp);
}

const bool ACGKnight::IsFenMatches(const TCHAR& iChr) const
{
	int idx;
	return KnightFen.FindChar(iChr, idx);
}

const FString ACGKnight::GetFenChar() const
{
	return IsWhite() ? TEXT("N") : TEXT("n");
}

void ACGKnight::RefreshMaterial()
{
	Super::RefreshMaterial();
	if (IsWhite())
	{
		Mesh->SetRelativeRotation(FRotator(0, WhiteMeshRotation, 0));
	}
	else
	{
		Mesh->SetRelativeRotation(FRotator(0, BlackMeshRotation, 0));
	}
}


