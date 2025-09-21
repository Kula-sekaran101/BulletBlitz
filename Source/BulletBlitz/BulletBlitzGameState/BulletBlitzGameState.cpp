//#include "BulletBlitzGameState.h"
//#include "Net/UnrealNetwork.h"
//
//ABulletBlitzGameState::ABulletBlitzGameState()
//{
//    bReplicates = true;
//    RoomCode = "";
//}
//
//void ABulletBlitzGameState::SetRoomCode(const FString& NewRoomCode)
//{
//    if (HasAuthority())
//    {
//        RoomCode = NewRoomCode;
//    }
//}
//
//void ABulletBlitzGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//    DOREPLIFETIME(ABulletBlitzGameState, RoomCode);
//}
