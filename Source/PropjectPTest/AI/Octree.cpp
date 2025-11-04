#include "AI/Octree.h"
#include "DrawDebugHelpers.h"

FGameOctree::FGameOctree(const FBox& InBounds, int32 InMaxElements)
    : Bounds(InBounds), MaxElements(InMaxElements)
{
}

FGameOctree::~FGameOctree()
{
}

void FGameOctree::Insert(const FGameOctreeElement& Element)
{
    if (!Bounds.IsInside(Element.Bounds.Origin)) return;

    if (bIsLeafNode && Elements.Num() < MaxElements)
    {
        Elements.Add(Element);
        return;
    }

    if (bIsLeafNode) Subdivide();

    for (const auto& Elem : Elements)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (Children[i]->Bounds.IsInside(Elem.Bounds.Origin))
            {
                Children[i]->Insert(Elem);
                break;
            }
        }
    }
    Elements.Empty();

    for (int i = 0; i < 8; ++i)
    {
        if (Children[i]->Bounds.IsInside(Element.Bounds.Origin))
        {
            Children[i]->Insert(Element);
            return;
        }
    }
}

void FGameOctree::Query(const FSphere& QuerySphere, TArray<AActor*>& OutActors) const
{
    if (!Bounds.Intersect(FBox::BuildAABB(QuerySphere.Center, FVector(QuerySphere.W)))) return;

    for (const auto& Elem : Elements)
    {
        if (QuerySphere.Intersects(Elem.Bounds.GetSphere()))
        {
            OutActors.Add(Elem.Actor);
        }
    }

    if (!bIsLeafNode)
    {
        for (int i = 0; i < 8; ++i)
        {
            Children[i]->Query(QuerySphere, OutActors);
        }
    }
}

void FGameOctree::DrawDebug(UWorld* World, int32 Depth) const
{
    if (!World) return;
    FColor DebugColor = FColor::White;
    switch (Depth % 6)
    {
        case 0: DebugColor = FColor::Red; break;
        case 1: DebugColor = FColor::Orange; break;
        case 2: DebugColor = FColor::Yellow; break;
        case 3: DebugColor = FColor::Green; break;
        case 4: DebugColor = FColor::Blue; break;
        case 5: DebugColor = FColor::Purple; break;
    }
    DrawDebugBox(World, Bounds.GetCenter(), Bounds.GetExtent(), DebugColor, false, -1, 0, 1.0f);

    if (!bIsLeafNode)
    {
        for (int i = 0; i < 8; ++i)
        {
            Children[i]->DrawDebug(World, Depth + 1);
        }
    }
}

void FGameOctree::Subdivide()
{
    // 현재 노드의 중심점과 자식 노드가 가질 크기(기존 크기의 절반)를 계산
    const FVector Center = Bounds.GetCenter();
    const FVector HalfExtent = Bounds.GetExtent() * 0.5f;

    // 8개의 자식 노드 경계를 계산하여 생성
    // 각 자식은 부모 공간의 8개 구역 중 하나를 차지
    // FBox는 Min 지점과 Max 지점으로 정의됨

    FVector Min = Bounds.Min;
    FVector Max = Bounds.Max;

    // 1. 뒤쪽-하단-왼쪽 (Back-Bottom-Left)
    Children[0] = MakeUnique<FGameOctree>(FBox(Min, Center), MaxElements);

    // 2. 뒤쪽-하단-오른쪽 (Back-Bottom-Right)
    Children[1] = MakeUnique<FGameOctree>(FBox(FVector(Center.X, Min.Y, Min.Z), FVector(Max.X, Center.Y, Center.Z)), MaxElements);

    // 3. 뒤쪽-상단-왼쪽 (Back-Top-Left)
    Children[2] = MakeUnique<FGameOctree>(FBox(FVector(Min.X, Min.Y, Center.Z), FVector(Center.X, Center.Y, Max.Z)), MaxElements);

    // 4. 뒤쪽-상단-오른쪽 (Back-Top-Right)
    Children[3] = MakeUnique<FGameOctree>(FBox(FVector(Center.X, Min.Y, Center.Z), FVector(Max.X, Center.Y, Max.Z)), MaxElements);

    // 5. 앞쪽-하단-왼쪽 (Front-Bottom-Left)
    Children[4] = MakeUnique<FGameOctree>(FBox(FVector(Min.X, Center.Y, Min.Z), FVector(Center.X, Max.Y, Center.Z)), MaxElements);

    // 6. 앞쪽-하단-오른쪽 (Front-Bottom-Right)
    Children[5] = MakeUnique<FGameOctree>(FBox(FVector(Center.X, Center.Y, Min.Z), FVector(Max.X, Max.Y, Center.Z)), MaxElements);

    // 7. 앞쪽-상단-왼쪽 (Front-Top-Left)
    Children[6] = MakeUnique<FGameOctree>(FBox(FVector(Min.X, Center.Y, Center.Z), FVector(Center.X, Max.Y, Max.Z)), MaxElements);

    // 8. 앞쪽-상단-오른쪽 (Front-Top-Right)
    Children[7] = MakeUnique<FGameOctree>(FBox(Center, Max), MaxElements);

    // 자식 노드가 생성되었으므로, 현재 노드는 더 이상 리프 노드가 아님
    bIsLeafNode = false;
}


void FGameOctree::GetAllElements(TArray<FGameOctreeElement>& OutElements) const
{
    // 현재 노드의 요소들을 추가
    OutElements.Append(Elements);

    // 리프 노드가 아니라면 자식 노드의 요소들도 재귀적으로 수집
    if (!bIsLeafNode)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (Children[i].IsValid())
            {
                Children[i]->GetAllElements(OutElements);
            }
        }
    }
}