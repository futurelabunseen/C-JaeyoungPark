#pragma once

#include "CoreMinimal.h"

struct FGameOctreeElement
{
    AActor* Actor;
    FBoxSphereBounds Bounds;

    FGameOctreeElement(AActor* InActor)
    {
        Actor = InActor;
        if (Actor)
        {
            const float ActorRadius = 100.0f;
            Bounds = FBoxSphereBounds(Actor->GetActorLocation(), FVector(ActorRadius), ActorRadius);
        }
    }

    // 복사 생성자/할당 연산자 필요시 추가 (FGameOctreeElement는 단순 구조체라 기본으로도 충분할 것)
    FGameOctreeElement() : Actor(nullptr) {} // 기본 생성자 추가하여 TArray가 사용 가능하게 함
};

class FGameOctree
{
public:
    FGameOctree(const FBox& InBounds, int32 InMaxElements);
    ~FGameOctree();

    void Insert(const FGameOctreeElement& Element);
    void Query(const FSphere& QuerySphere, TArray<AActor*>& OutActors) const;
    void DrawDebug(UWorld* World, int32 Depth = 0) const;

    // ◀◀◀ 모든 요소를 재귀적으로 수집하는 함수를 추가합니다.
    void GetAllElements(TArray<FGameOctreeElement>& OutElements) const;

private:
    void Subdivide();

private:
    FBox Bounds;
    int32 MaxElements;
    TArray<FGameOctreeElement> Elements;
    TUniquePtr<FGameOctree> Children[8];
    bool bIsLeafNode = true;
};