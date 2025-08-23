#include "network_screen.hpp"
#include "../components/ui_card.hpp"
#include "../components/graph_component.hpp"

void NetworkScreen::RenderContent(ClayMan* clayMan, const SystemState& systemState)
{
    // Get window dimensions for responsive layout
    int windowWidth = clayMan->getWindowWidth();
    int windowHeight = clayMan->getWindowHeight();
    
    // Calculate responsive spacing
    uint32_t sectionGap = std::max(20u, static_cast<uint32_t>(windowHeight * 0.025f));
    
    Clay_ElementDeclaration networkContainer = {};
    // Fill available width and allow vertical growth
    networkContainer.layout.sizing = clayMan->expandXY();
    networkContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    networkContainer.layout.childGap = sectionGap;
    
    clayMan->element(networkContainer, [this, clayMan, &systemState, windowWidth]() {
        RenderNetworkSummary(clayMan, systemState, windowWidth);
        RenderNetworkGraphs(clayMan, systemState);
        RenderNetworkInterfaces(clayMan, systemState.networkStats);
    });
}

void NetworkScreen::RenderNetworkSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth)
{
    uint32_t cardHeight = 140;
    
    UICard::RenderSectionCard(clayMan, "Network Activity Overview", [this, clayMan, &systemState, cardHeight, windowWidth]() {
        
        Clay_ElementDeclaration summaryContainer = {};
        summaryContainer.layout.sizing = clayMan->expandXfixedY(cardHeight);
        summaryContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        summaryContainer.layout.childGap = std::max(16u, static_cast<uint32_t>(windowWidth * 0.02f));
        
        clayMan->element(summaryContainer, [this, clayMan, &systemState, cardHeight]() {
            
            // Network Usage Card
            UICard::RenderMetricCard(clayMan, "Network Usage",
                FormatPercentage(systemState.networkUsagePercent),
                "Current utilization",
                { 100, 150, 255, 255 }, cardHeight);
            
            // Download Speed Card
            UICard::RenderMetricCard(clayMan, "Download Speed",
                FormatBytes(systemState.downloadBytesPerSec) + "/s",
                "Incoming data rate",
                { 0, 255, 150, 255 }, cardHeight);
            
            // Upload Speed Card
            UICard::RenderMetricCard(clayMan, "Upload Speed",
                FormatBytes(systemState.uploadBytesPerSec) + "/s",
                "Outgoing data rate",
                { 255, 150, 0, 255 }, cardHeight);
            
            // Active Connections Card
            UICard::RenderMetricCard(clayMan, "Active Connections",
                std::to_string(systemState.networkStats.activeConnections),
                "Open network connections",
                { 255, 255, 100, 255 }, cardHeight);
        });
        
    }, cardHeight + 80);
}

void NetworkScreen::RenderNetworkGraphs(ClayMan* clayMan, const SystemState& systemState)
{
    uint32_t graphHeight = std::max(300u, static_cast<uint32_t>(clayMan->getWindowHeight() * 0.35f));
    
    UICard::RenderSectionCard(clayMan, "Network Performance Graphs", [this, clayMan, &systemState, graphHeight]() {
        
    Clay_ElementDeclaration graphsLayout = {};
    // Fill available space and allow growing for responsiveness
    graphsLayout.layout.sizing = clayMan->expandXY();
        graphsLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        graphsLayout.layout.childGap = 20;
        
        clayMan->element(graphsLayout, [this, clayMan, &systemState, graphHeight]() {
            
            // Combined Upload/Download Graph
            RenderCombinedNetworkGraph(clayMan, systemState, graphHeight - 50);
            
            // Separate graphs row or column depending on width
            Clay_ElementDeclaration separateGraphsRow = {};
            separateGraphsRow.layout.sizing = clayMan->expandXfixedY(graphHeight - 50);
            // If window narrow, stack vertically
            if (clayMan->getWindowWidth() < (graphHeight - 50) * 2) {
                separateGraphsRow.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            } else {
                separateGraphsRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            }
            separateGraphsRow.layout.childGap = 20;
            
            clayMan->element(separateGraphsRow, [this, clayMan, &systemState, graphHeight]() {
                
                // Download Graph
                UICard::RenderWithBackground(clayMan, [clayMan, &systemState, graphHeight]() {
                    Clay_ElementDeclaration downloadCard = {};
                    downloadCard.layout.sizing = clayMan->expandXY();
                    downloadCard.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
                    downloadCard.layout.padding = clayMan->padAll(16);
                    downloadCard.layout.childGap = 12;
                    
                    clayMan->element(downloadCard, [clayMan, &systemState, graphHeight]() {
                        // Title
                        Clay_ElementDeclaration titleContainer = {};
                        titleContainer.layout.sizing = clayMan->expandXfixedY(30);
                        titleContainer.layout.childAlignment = clayMan->centerXY();
                        
                        clayMan->element(titleContainer, [clayMan]() {
                            Clay_TextElementConfig titleText = {};
                            titleText.textColor = { 220, 220, 220, 255 };
                            titleText.fontId = 0;
                            titleText.fontSize = 16;
                            clayMan->textElement("Download Activity", titleText);
                        });
                        
                        // Graph
                        Clay_ElementDeclaration graphContainer = {};
                        graphContainer.layout.sizing = clayMan->expandXY();
                        
                        clayMan->element(graphContainer, [clayMan, &systemState, graphHeight]() {
                            // Convert download history from uint64_t to float for visualization
                            RingBuffer<float, 300> downloadFloatHistory;
                            for (size_t i = 0; i < systemState.downloadHistory.size(); ++i) {
                                downloadFloatHistory.push(static_cast<float>(systemState.downloadHistory.get(i) / 1024.0f)); // Convert to KB/s
                            }
                            GraphComponent::RenderTimeSeriesGraph(clayMan, downloadFloatHistory, 
                                { 0, 255, 150, 255 }, graphHeight - 100);
                        });
                    });
                }, { 35, 35, 35, 255 }, 0, 0, 12);
                
                // Upload Graph
                UICard::RenderWithBackground(clayMan, [clayMan, &systemState, graphHeight]() {
                    Clay_ElementDeclaration uploadCard = {};
                    uploadCard.layout.sizing = clayMan->expandXY();
                    uploadCard.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
                    uploadCard.layout.padding = clayMan->padAll(16);
                    uploadCard.layout.childGap = 12;
                    
                    clayMan->element(uploadCard, [clayMan, &systemState, graphHeight]() {
                        // Title
                        Clay_ElementDeclaration titleContainer = {};
                        titleContainer.layout.sizing = clayMan->expandXfixedY(30);
                        titleContainer.layout.childAlignment = clayMan->centerXY();
                        
                        clayMan->element(titleContainer, [clayMan]() {
                            Clay_TextElementConfig titleText = {};
                            titleText.textColor = { 220, 220, 220, 255 };
                            titleText.fontId = 0;
                            titleText.fontSize = 16;
                            clayMan->textElement("Upload Activity", titleText);
                        });
                        
                        // Graph
                        Clay_ElementDeclaration graphContainer = {};
                        graphContainer.layout.sizing = clayMan->expandXY();
                        
                        clayMan->element(graphContainer, [clayMan, &systemState, graphHeight]() {
                            // Convert upload history from uint64_t to float for visualization
                            RingBuffer<float, 300> uploadFloatHistory;
                            for (size_t i = 0; i < systemState.uploadHistory.size(); ++i) {
                                uploadFloatHistory.push(static_cast<float>(systemState.uploadHistory.get(i) / 1024.0f)); // Convert to KB/s
                            }
                            GraphComponent::RenderTimeSeriesGraph(clayMan, uploadFloatHistory, 
                                { 255, 150, 0, 255 }, graphHeight - 100);
                        });
                    });
                }, { 35, 35, 35, 255 }, 0, 0, 12);
            });
        });
        
    }, (graphHeight * 2) + 120);
}

void NetworkScreen::RenderCombinedNetworkGraph(ClayMan* clayMan, const SystemState& systemState, uint32_t graphHeight)
{
    UICard::RenderWithBackground(clayMan, [clayMan, &systemState, graphHeight, this]() {
        Clay_ElementDeclaration combinedCard = {};
        combinedCard.layout.sizing = clayMan->expandXfixedY(graphHeight);
        combinedCard.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        combinedCard.layout.padding = clayMan->padAll(16);
        combinedCard.layout.childGap = 12;
        
        clayMan->element(combinedCard, [clayMan, &systemState, graphHeight, this]() {
            // Title and legend
            Clay_ElementDeclaration headerSection = {};
            headerSection.layout.sizing = clayMan->expandXfixedY(40);
            headerSection.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            headerSection.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(headerSection, [clayMan, this]() {
                // Title
                Clay_ElementDeclaration titleContainer = {};
                titleContainer.layout.sizing = clayMan->expandXY();
                titleContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(titleContainer, [clayMan]() {
                    Clay_TextElementConfig titleText = {};
                    titleText.textColor = { 220, 220, 220, 255 };
                    titleText.fontId = 0;
                    titleText.fontSize = 18;
                    clayMan->textElement("Network Traffic Overview", titleText);
                });
                
                // Legend
                Clay_ElementDeclaration legendContainer = {};
                legendContainer.layout.sizing = clayMan->fixedSize(200, 40);
                legendContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
                legendContainer.layout.childGap = 16;
                legendContainer.layout.childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(legendContainer, [clayMan, this]() {
                    // Download legend
                    this->RenderLegendItem(clayMan, "Download", { 0, 255, 150, 255 });
                    // Upload legend
                    this->RenderLegendItem(clayMan, "Upload", { 255, 150, 0, 255 });
                });
            });
            
            // Combined graph using network usage history
            Clay_ElementDeclaration graphContainer = {};
            graphContainer.layout.sizing = clayMan->expandXY();
            
            clayMan->element(graphContainer, [clayMan, &systemState, graphHeight]() {
                GraphComponent::RenderTimeSeriesGraph(clayMan, systemState.networkHistory, 
                    { 100, 150, 255, 255 }, graphHeight - 80);
            });
        });
    }, { 40, 40, 40, 255 }, graphHeight, 0, 16);
}

void NetworkScreen::RenderLegendItem(ClayMan* clayMan, const std::string& label, Clay_Color color)
{
    Clay_ElementDeclaration legendItem = {};
    legendItem.layout.sizing = clayMan->fixedSize(80, 24);
    legendItem.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    legendItem.layout.childGap = 6;
    legendItem.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
    
    clayMan->element(legendItem, [clayMan, &label, color]() {
        // Color indicator
        Clay_ElementDeclaration colorBox = {};
        colorBox.layout.sizing = clayMan->fixedSize(12, 12);
        colorBox.backgroundColor = color;
        colorBox.cornerRadius = { 2, 2, 2, 2 };
        clayMan->element(colorBox, []() {});
        
        // Label
        Clay_ElementDeclaration labelContainer = {};
        labelContainer.layout.sizing = clayMan->expandXY();
        labelContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
        clayMan->element(labelContainer, [clayMan, &label]() {
            Clay_TextElementConfig labelText = {};
            labelText.textColor = { 180, 180, 180, 255 };
            labelText.fontId = 0;
            labelText.fontSize = 12;
            clayMan->textElement(label, labelText);
        });
    });
}

void NetworkScreen::RenderNetworkInterfaces(ClayMan* clayMan, const NetworkStats& networkStats)
{
    UICard::RenderSectionCard(clayMan, "Network Interfaces", [this, clayMan, &networkStats]() {
        
        Clay_ElementDeclaration interfacesContainer = {};
        interfacesContainer.layout.sizing = clayMan->expandXY();
        interfacesContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        interfacesContainer.layout.childGap = 12;
        
        clayMan->element(interfacesContainer, [this, clayMan, &networkStats]() {
            
            if (networkStats.interfaces.empty()) {
                // No interfaces message
                Clay_ElementDeclaration noDataContainer = {};
                noDataContainer.layout.sizing = clayMan->expandXfixedY(80);
                noDataContainer.layout.childAlignment = clayMan->centerXY();
                
                clayMan->element(noDataContainer, [clayMan]() {
                    Clay_TextElementConfig noDataText = {};
                    noDataText.textColor = { 150, 150, 150, 255 };
                    noDataText.fontId = 0;
                    noDataText.fontSize = 14;
                    clayMan->textElement("No network interfaces detected", noDataText);
                });
            } else {
                for (const auto& interface : networkStats.interfaces) {
                    RenderInterfaceCard(clayMan, interface);
                }
            }
        });
        
    }, 0); // Auto height
}

void NetworkScreen::RenderInterfaceCard(ClayMan* clayMan, const NetworkInterface& interface)
{
    UICard::RenderWithBackground(clayMan, [clayMan, &interface, this]() {
        
        Clay_ElementDeclaration interfaceLayout = {};
        interfaceLayout.layout.sizing = clayMan->expandXfixedY(120);
        interfaceLayout.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        interfaceLayout.layout.padding = clayMan->padAll(16);
        interfaceLayout.layout.childGap = 16;
        
        clayMan->element(interfaceLayout, [clayMan, &interface, this]() {
            
            // Interface info section
            Clay_ElementDeclaration infoSection = {};
            infoSection.layout.sizing = clayMan->expandXY();
            infoSection.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            infoSection.layout.childGap = 8;
            
            clayMan->element(infoSection, [clayMan, &interface, this]() {
                
                // Interface name and status
                Clay_ElementDeclaration headerRow = {};
                headerRow.layout.sizing = clayMan->expandXfixedY(24);
                headerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
                headerRow.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                headerRow.layout.childGap = 12;
                
                clayMan->element(headerRow, [clayMan, &interface]() {
                    // Interface name
                    Clay_ElementDeclaration nameContainer = {};
                    nameContainer.layout.sizing = clayMan->expandXY();
                    nameContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                    
                    clayMan->element(nameContainer, [clayMan, &interface]() {
                        Clay_TextElementConfig nameText = {};
                        nameText.textColor = { 220, 220, 220, 255 };
                        nameText.fontId = 0;
                        nameText.fontSize = 16;
                        clayMan->textElement(interface.name, nameText);
                    });
                    
                    // Connection status
                    Clay_ElementDeclaration statusBadge = {};
                    statusBadge.layout.sizing = clayMan->fixedSize(80, 20);
                    statusBadge.backgroundColor = interface.isConnected ? 
                        Clay_Color{ 0, 255, 150, 255 } : Clay_Color{ 255, 100, 100, 255 };
                    statusBadge.cornerRadius = { 10, 10, 10, 10 };
                    statusBadge.layout.childAlignment = clayMan->centerXY();
                    
                    clayMan->element(statusBadge, [clayMan, &interface]() {
                        Clay_TextElementConfig statusText = {};
                        statusText.textColor = { 25, 25, 25, 255 };
                        statusText.fontId = 0;
                        statusText.fontSize = 10;
                        clayMan->textElement(interface.isConnected ? "Connected" : "Disconnected", statusText);
                    });
                });
                
                // Interface details
                this->RenderInterfaceDetails(clayMan, interface);
            });
            
            // Statistics section
            Clay_ElementDeclaration statsSection = {};
            statsSection.layout.sizing = clayMan->fixedSize(200, 120);
            statsSection.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            statsSection.layout.childGap = 4;
            
            clayMan->element(statsSection, [clayMan, &interface, this]() {
                this->RenderInterfaceStats(clayMan, interface);
            });
        });
        
    }, { 35, 35, 35, 255 }, 120, 12, 12);
}

void NetworkScreen::RenderInterfaceDetails(ClayMan* clayMan, const NetworkInterface& interface)
{
    Clay_ElementDeclaration detailsContainer = {};
    detailsContainer.layout.sizing = clayMan->expandXY();
    detailsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    detailsContainer.layout.childGap = 4;
    
    clayMan->element(detailsContainer, [clayMan, &interface]() {
        
        // Type and speed
        Clay_ElementDeclaration typeRow = {};
        typeRow.layout.sizing = clayMan->expandXfixedY(16);
        typeRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        typeRow.layout.childGap = 8;
        
        clayMan->element(typeRow, [clayMan, &interface]() {
            Clay_TextElementConfig labelText = {};
            labelText.textColor = { 160, 160, 160, 255 };
            labelText.fontId = 0;
            labelText.fontSize = 12;
            clayMan->textElement("Type:", labelText);
            
            Clay_TextElementConfig valueText = {};
            valueText.textColor = { 200, 200, 200, 255 };
            valueText.fontId = 0;
            valueText.fontSize = 12;
            clayMan->textElement(interface.type + " (" + std::to_string(interface.speed) + " Mbps)", valueText);
        });
        
        // IP Address
        if (!interface.ipAddress.empty()) {
            Clay_ElementDeclaration ipRow = {};
            ipRow.layout.sizing = clayMan->expandXfixedY(16);
            ipRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            ipRow.layout.childGap = 8;
            
            clayMan->element(ipRow, [clayMan, &interface]() {
                Clay_TextElementConfig labelText = {};
                labelText.textColor = { 160, 160, 160, 255 };
                labelText.fontId = 0;
                labelText.fontSize = 12;
                clayMan->textElement("IP:", labelText);
                
                Clay_TextElementConfig valueText = {};
                valueText.textColor = { 100, 150, 255, 255 };
                valueText.fontId = 0;
                valueText.fontSize = 12;
                clayMan->textElement(interface.ipAddress, valueText);
            });
        }
        
        // MAC Address
        if (!interface.macAddress.empty()) {
            Clay_ElementDeclaration macRow = {};
            macRow.layout.sizing = clayMan->expandXfixedY(16);
            macRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            macRow.layout.childGap = 8;
            
            clayMan->element(macRow, [clayMan, &interface]() {
                Clay_TextElementConfig labelText = {};
                labelText.textColor = { 160, 160, 160, 255 };
                labelText.fontId = 0;
                labelText.fontSize = 12;
                clayMan->textElement("MAC:", labelText);
                
                Clay_TextElementConfig valueText = {};
                valueText.textColor = { 180, 180, 180, 255 };
                valueText.fontId = 0;
                valueText.fontSize = 11;
                clayMan->textElement(interface.macAddress, valueText);
            });
        }
    });
}

void NetworkScreen::RenderInterfaceStats(ClayMan* clayMan, const NetworkInterface& interface)
{
    std::vector<std::pair<std::string, std::string>> stats = {
        {"Bytes Received", FormatBytes(interface.bytesReceived)},
        {"Bytes Sent", FormatBytes(interface.bytesSent)},
        {"Packets Received", std::to_string(interface.packetsReceived)},
        {"Packets Sent", std::to_string(interface.packetsSent)}
    };
    
    for (const auto& stat : stats) {
        Clay_ElementDeclaration statRow = {};
        statRow.layout.sizing = clayMan->expandXfixedY(20);
        statRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        statRow.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        statRow.backgroundColor = { 25, 25, 25, 255 };
        statRow.cornerRadius = { 4, 4, 4, 4 };
        statRow.layout.padding = clayMan->padXY(8, 4);
        
        clayMan->element(statRow, [clayMan, &stat]() {
            // Label
            Clay_ElementDeclaration labelContainer = {};
            labelContainer.layout.sizing = clayMan->fixedSize(80, 20);
            labelContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(labelContainer, [clayMan, &stat]() {
                Clay_TextElementConfig labelText = {};
                labelText.textColor = { 140, 140, 140, 255 };
                labelText.fontId = 0;
                labelText.fontSize = 10;
                clayMan->textElement(stat.first, labelText);
            });
            
            // Value
            Clay_ElementDeclaration valueContainer = {};
            valueContainer.layout.sizing = clayMan->expandXY();
            valueContainer.layout.childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(valueContainer, [clayMan, &stat]() {
                Clay_TextElementConfig valueText = {};
                valueText.textColor = { 200, 200, 200, 255 };
                valueText.fontId = 0;
                valueText.fontSize = 10;
                clayMan->textElement(stat.second, valueText);
            });
        });
    }
}