#pragma once

void initCommunicationRxTx();
void handleCommunicationRxTx();
void sendCommunicationRxTxMessage(const char *message);
bool hasReceivedSelesai();
void clearReceivedSelesai();