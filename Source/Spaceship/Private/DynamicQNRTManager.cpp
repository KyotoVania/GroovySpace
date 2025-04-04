#include "DynamicQNRTManager.h"
#include "UObject/UnrealType.h" // Pour FPropertyChangedEvent si n�cessaire

void USoundPropertyManager::SetSound(UConstantQNRT* ConstantQNRT, USoundWave* InSound)
{
    if (!ConstantQNRT)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetSound: ConstantQNRT is null. Cannot set sound."));
        return;
    }

    if (!InSound)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetSound: InSound is null. Cannot set sound."));
        return;
    }

    // Assign the new sound
    ConstantQNRT->Sound = InSound;
    UE_LOG(LogTemp, Log, TEXT("SetSound: Sound set to %s"), *InSound->GetName());

    // Ensure Settings are valid
    if (!ConstantQNRT->Settings)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetSound: ConstantQNRT Settings are null. Creating default settings."));
        ConstantQNRT->Settings = NewObject<UConstantQNRTSettings>(ConstantQNRT);
    }

    // Force reanalyze after assigning the sound
    ForceReanalyze(ConstantQNRT);
}

USoundWave* USoundPropertyManager::GetSound(UConstantQNRT* ConstantQNRT)
{
    if (!ConstantQNRT)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSound: ConstantQNRT is null. Returning nullptr."));
        return nullptr;
    }

    if (!ConstantQNRT->Sound)
    {
        UE_LOG(LogTemp, Warning, TEXT("GetSound: No sound is assigned to this ConstantQNRT."));
        return nullptr;
    }

    return ConstantQNRT->Sound;
}

void USoundPropertyManager::ForceReanalyze(UConstantQNRT* ConstantQNRT)
{
    if (!ConstantQNRT)
    {
        UE_LOG(LogTemp, Warning, TEXT("ForceReanalyze: ConstantQNRT is null. Cannot force reanalyze."));
        return;
    }

    if (!ConstantQNRT->Settings)
    {
        UE_LOG(LogTemp, Warning, TEXT("ForceReanalyze: ConstantQNRT Settings are null. Cannot reanalyze."));
        return;
    }

    // Simulate a property change to force reanalysis
    UE_LOG(LogTemp, Log, TEXT("ForceReanalyze: Triggering analysis recalculation for sound: %s"), 
           *ConstantQNRT->Sound->GetName());

    ConstantQNRT->Settings->StartingFrequency += 1.0f; // Arbitrary modification
    ConstantQNRT->Settings->StartingFrequency -= 1.0f; // Restore original value

    FPropertyChangedEvent PropertyChangedEvent(FindFieldChecked<FProperty>(
        UConstantQNRT::StaticClass(), GET_MEMBER_NAME_CHECKED(UConstantQNRT, Sound)));

    ConstantQNRT->Modify(); // Indique que l'objet a été modifié
    ConstantQNRT->MarkPackageDirty(); // Marque le package comme modifié
    //print tis data UE_LOG(LogTemp, Log, TEXT("RawPCMDataSize: %d"), ImportedSoundWave->RawPCMDataSize);
  //  UE_LOG(LogTemp, Log, TEXT("Duration: %f"), ImportedSoundWave->Duration);
    
    UE_LOG(LogTemp, Log, TEXT("ForceReanalyze: Analysis recalculated successfully."));
}
