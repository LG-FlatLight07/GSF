#include "GSFAbility.h"

#if WITH_EDITOR
#include "Misc/MessageDialog.h"
#include "UObject/Package.h"
#include "EditorReimportHandler.h"
#include "FileHelpers.h"
#endif

#define LOCTEXT_NAMESPACE "TEST"

void UAbilityDataAsset::Build()
{
#if WITH_EDITORONLY_DATA
 
    // データテーブルの設定チェック
    if( dataTable == nullptr)
    {
        const FText TitleText = LOCTEXT( "Title", "WarningMassege" );
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT( "Message", "DataTable is Null !!" ), &TitleText );
        return;
    }
 
    // データテーブルの型チェック
    if( !dataTable->GetRowStruct()->IsChildOf( FAbilityData_TableRow::StaticStruct() ) )
    {
        const FText titleText = LOCTEXT( "Title", "WarningMassege" );
        FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "Message", "DataTable type does not match !!" ), &titleText );
        return;
    }
    
    TArray<UPackage*> packagesToSave;
 
    // csvではなくdataTableを直接変更したときは一度エクスポートする
    if(dataTable->GetOutermost()->IsDirty())
    {
        FString SaveDirectory = FPaths::ProjectContentDir() + TEXT("DataTable/");
        FString CSVFilePath = SaveDirectory + TEXT("AbilityData.csv");
        FFileHelper::SaveStringToFile(dataTable->GetTableAsCSV(), *CSVFilePath);
    }
    // データテーブルをリインポート
    if( FReimportManager::Instance()->Reimport( dataTable, false, true ) )
    {
        // リインポートに成功したデータテーブルを保存対象に追加
        packagesToSave.Add( dataTable->GetOutermost() );
    }
    
    // データを一時避難
    for( auto Data : abilityDataMap )
    {
        SaveData->Save(Data.Key, Data.Value);
    }
    
    abilityDataMap.Empty();
    
    // データテーブルの行の要素を配列で取得
    TArray RowNames = dataTable->GetRowNames();
    // 上書き
    for( auto RowName : RowNames )
    {
        // １行分の構造体を取得
        FAbilityData_TableRow* TableRow = dataTable->FindRow< FAbilityData_TableRow >( RowName, FString() );
 
        // 実際にゲーム上で使いやすいようにデータを加工する
        // 一部のデータはCSVで管理できないためエディター側で設定する
        FAbilityData abilityData;
        abilityData.AbilityID = TableRow->AbilityID;
        abilityData.AbilityName = RowName.ToString();
        abilityData.Damage = TableRow->Damage;
        abilityData.Force = TableRow->Force;
        abilityData.ForceVector = TableRow->ForceVector;
        abilityData.FearTime = TableRow->FearTime;
        abilityData.FrontGap = TableRow->FrontGap;
        abilityData.Duration = TableRow->Duration;
        abilityData.RearGap = TableRow->RearGap;
        abilityData.AfterOver = TableRow->AfterOver;
        
        // 保存されたデータがあれば適用
        FAbilityData_TemporaryEvacuation save;
        if(SaveData->Load(TableRow->AbilityID, save))
        {
            abilityData.Montage = save.Montage;
            abilityData.HitEffect = save.HitEffect;
        }
 
        // Mapに追加する
        abilityDataMap.Add( TableRow->AbilityID, abilityData );
    }

 
    // データアセットに編集フラグを追加
    MarkPackageDirty();
 
    // データアセットを保存対象に追加
    packagesToSave.Add( GetOutermost() );
 
    // 関連アセットを全て保存（SourceControl使用時はチェックアウトするかメッセージウィンドウを出す）
    // ファイル編集フラグ（Dirty)が付いてるもののみを保存対象にしたいので第一引数はtrue
    // 保存する際に確認のメッセージウィンドウを出さない場合は第二引数をfalseにする
    FEditorFileUtils::PromptForCheckoutAndSave( packagesToSave, true, true );
 
#endif
}
#undef LOCTEXT_NAMESPACE

// データを避難
void UAbilityDataAsset_TemporaryEvacuation::Save(const int ID, const FAbilityData Data)
{
    FAbilityData_TemporaryEvacuation abilityData;
    abilityData.Montage = Data.Montage;
    abilityData.HitEffect = Data.HitEffect;
    abilitySaveDataMap.Add(ID, abilityData);
}

// 避難させたデータを取得
bool UAbilityDataAsset_TemporaryEvacuation::Load(const int ID, FAbilityData_TemporaryEvacuation& retData)const
{
    if(abilitySaveDataMap.Contains(ID))
    {
        retData = abilitySaveDataMap[ID];
        return true;
    }
    return false;
}
