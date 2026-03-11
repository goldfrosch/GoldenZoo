# BeaverCrafting

`BeaverCrafting`은 Unreal Engine용 제작 레시피 플러그인입니다. 현재 구현 범위는 크게 두 가지입니다.

- 런타임: 재료 조합을 토큰화해서 레시피를 정확 일치 방식으로 검색하는 인덱스
- 에디터: `FBCraftingRecipeRow` 기반 `DataTable`을 빠르게 검증할 수 있는 툴바 확장과 트리 시각화 탭

## 현재 제공 기능

### 1. 레시피 데이터 구조

플러그인은 `DataTable` 행 구조로 `FBCraftingRecipeRow`를 제공합니다.

- `ResultId`: 제작 결과 ID
- `ProductType`: 결과물 타입
- `IngredientClauses`: 재료 목록
- `Priority`: 동일 조건 레시피가 여러 개일 때 우선순위

재료 항목은 `FBCraftingIngredientClause`로 정의되며 다음 값을 가집니다.

- `Key`: 재료 ID
- `Count`: 필요 수량

런타임 검색용 입력 구조로는 `FBCraftingRecipeQuery`와 `FBCraftingSelectionIngredient`가 제공됩니다.

### 2. 재료 조합 정규화

레시피와 검색 쿼리는 모두 내부적으로 검색 토큰 배열로 변환됩니다.

- `ProductType`
- `MaterialId`
- `MaterialCount`

이 과정에서 다음 정규화가 적용됩니다.

- 같은 재료가 여러 번 들어오면 수량을 합산
- 재료 ID 기준으로 정렬하여 입력 순서 차이를 제거
- 수량은 최소 1 이상으로 보정

즉, 재료 입력 순서가 달라도 같은 조합이면 동일한 검색 키로 처리됩니다.

### 3. 정확 일치 레시피 검색 인덱스

`FBCraftingRecipeIndex`는 토큰 시퀀스를 트리 형태로 저장합니다.

- `AddRecipe(...)`: 레시피를 인덱스에 추가
- `FindExact(...)`: `FBCraftingRecipeQuery` 기준 정확 일치 검색
- `FindExactTokens(...)`: 토큰 배열 기준 정확 일치 검색
- `Reset()`: 인덱스 초기화
- `GetRecipeCount()`: 인덱싱된 레시피 수 조회

검색 결과는 `FBCraftingRecipeMatch`로 반환됩니다.

- `bFound`
- `RecipeRowName`
- `ResultId`
- `Priority`

동일한 토큰 조합에 여러 레시피가 연결된 경우:

- `Priority`가 높은 레시피를 우선 선택
- `Priority`가 같으면 `RecipeRowName` 사전순으로 결정

### 4. DataTable에서 인덱스 자동 생성

`FBCraftingRecipeIndexBuilder::BuildFromDataTable(...)`를 통해 `UDataTable`에서 인덱스를 구성할 수 있습니다.

- `FBCraftingRecipeRow` 기반 행을 순회
- 각 행의 검색 토큰을 생성
- 결과 ID와 우선순위를 포함한 매치를 인덱스에 등록

별도 수작업 없이 레시피 테이블 전체를 한 번에 인덱싱할 수 있습니다.

### 5. 에디터 DataTable 툴바 확장

에디터 모듈은 `FBCraftingRecipeRow`를 행 구조로 사용하는 `DataTable` 편집기에 툴바 버튼을 추가합니다.

- 버튼 이름: `Refresh Recipe Tree`
- 동작: 현재 열린 레시피 테이블을 다시 읽고 인덱스를 재생성
- 완료 후 알림으로 테이블 이름, 전체 행 수, 인덱싱 수를 표시

이 버튼은 일반 `DataTable`이 아니라 BeaverCrafting 레시피 테이블일 때만 노출됩니다.

### 6. 레시피 인덱스 시각화 탭

에디터에는 `Recipe Index Visualizer` 탭이 등록됩니다.

- 인덱스를 트리 형태로 표시
- `ProductType`, `MaterialId`, `MaterialCount` 토큰 흐름 확인 가능
- 각 말단 노드에서 매칭되는 `Row`, `Result`, `Priority` 확인 가능
- 인덱스가 갱신되면 탭을 자동으로 열고 루트 항목을 펼쳐서 보여줌

레시피가 왜 특정 결과로 연결되는지 확인하거나, 중복 레시피 구조를 검토할 때 유용합니다.

## 모듈 구성

- `BeaverCrafting` (`Runtime`)
  - 레시피 타입
  - 검색 토큰 생성
  - 정확 일치 인덱스
  - DataTable 인덱스 빌더
- `BeaverCraftingEditor` (`Editor`)
  - DataTable 편집기 툴바 확장
  - 인덱스 시각화 탭

## 기본 사용 흐름

### 1. 레시피 테이블 생성

`Row Structure`를 `FBCraftingRecipeRow`로 지정한 `DataTable`을 만듭니다.

예시 필드:

```text
RowName: Recipe_Axe_WoodStone
ResultId: Axe
ProductType: Tool
IngredientClauses:
  - Key: Wood, Count: 2
  - Key: Stone, Count: 1
Priority: 10
```

### 2. 런타임에서 인덱스 생성

```cpp
FBCraftingRecipeIndex RecipeIndex;
FBCraftingRecipeIndexBuilder::BuildFromDataTable(RecipeTable, RecipeIndex);
```

### 3. 제작 쿼리 검색

```cpp
FBCraftingRecipeQuery Query;
Query.ProductType = TEXT("Tool");

FBCraftingSelectionIngredient& Wood = Query.Ingredients.AddDefaulted_GetRef();
Wood.MaterialId = TEXT("Wood");
Wood.Count = 2;

FBCraftingSelectionIngredient& Stone = Query.Ingredients.AddDefaulted_GetRef();
Stone.MaterialId = TEXT("Stone");
Stone.Count = 1;

FBCraftingRecipeMatch Match;
if (RecipeIndex.FindExact(Query, Match))
{
	// Match.ResultId 사용
}
```

### 4. 에디터에서 레시피 검증

1. 레시피 `DataTable`을 연다.
2. 툴바에서 `Refresh Recipe Tree`를 누른다.
3. `Recipe Index Visualizer` 탭에서 인덱스 구조를 확인한다.

## 현재 범위에서의 제한 사항

현재 코드 기준으로는 아래 기능이 아직 포함되어 있지 않습니다.

- 부분 일치, 유사 검색, 태그 기반 검색
- 조건식 평가, 확률 제작, 가변 결과물
- 인벤토리 차감이나 실제 제작 실행을 담당하는 서브시스템
- 블루프린트 전용 함수 라이브러리
- 네트워크 동기화, 저장/로드 통합

즉, 현 시점의 BeaverCrafting은 "레시피 데이터 정의 + 정확 일치 검색 + 에디터 검증 도구"에 초점이 맞춰져 있습니다.
