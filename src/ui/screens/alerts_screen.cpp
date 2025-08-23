#include "alerts_screen.hpp"
#include "../components/ui_card.hpp"
#include <algorithm>

void AlertsScreen::RenderContent(ClayMan* clayMan, const SystemState& systemState)
{
    // Get window dimensions for responsive layout
    int windowWidth = clayMan->getWindowWidth();
    int windowHeight = clayMan->getWindowHeight();
    
    // Calculate responsive spacing
    uint32_t sectionGap = std::max(20u, static_cast<uint32_t>(windowHeight * 0.025f));
    
    Clay_ElementDeclaration alertsContainer = {};
    // Fill available width and allow vertical growth
    alertsContainer.layout.sizing = clayMan->expandXY();
    alertsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    alertsContainer.layout.childGap = sectionGap;
    
    clayMan->element(alertsContainer, [this, clayMan, &systemState, windowWidth]() {
        RenderAlertsSummary(clayMan, systemState, windowWidth);
        RenderActiveAlerts(clayMan, systemState.activeAlerts);
        RenderAlertRules(clayMan, systemState.alertRules);
    });
}

void AlertsScreen::RenderAlertsSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth)
{
    uint32_t cardHeight = 140;
    
    UICard::RenderSectionCard(clayMan, "Alerts Overview", [this, clayMan, &systemState, cardHeight, windowWidth]() {
        
        Clay_ElementDeclaration summaryContainer = {};
        summaryContainer.layout.sizing = clayMan->expandXfixedY(cardHeight);
        summaryContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        summaryContainer.layout.childGap = std::max(16u, static_cast<uint32_t>(windowWidth * 0.02f));
        
        clayMan->element(summaryContainer, [this, clayMan, &systemState, cardHeight]() {
            
            // Active Alerts Count
            uint32_t activeAlertsCount = static_cast<uint32_t>(systemState.activeAlerts.size());
            UICard::RenderMetricCard(clayMan, "Active Alerts",
                std::to_string(activeAlertsCount),
                "Currently triggered alerts",
                activeAlertsCount > 0 ? Clay_Color{255, 100, 100, 255} : Clay_Color{0, 255, 150, 255},
                cardHeight);
            
            // Total Alert Rules
            uint32_t totalRules = static_cast<uint32_t>(systemState.alertRules.size());
            uint32_t enabledRules = 0;
            for (const auto& rule : systemState.alertRules) {
                if (rule.isEnabled) enabledRules++;
            }
            
            UICard::RenderMetricCard(clayMan, "Alert Rules",
                std::to_string(enabledRules) + "/" + std::to_string(totalRules),
                "Enabled monitoring rules",
                { 100, 150, 255, 255 }, cardHeight);
            
            // System Status
            std::string systemStatus = "Healthy";
            Clay_Color statusColor = { 0, 255, 150, 255 }; // Green
            
            // Determine system status based on active alerts
            uint32_t criticalAlerts = 0;
            uint32_t warningAlerts = 0;
            for (const auto& alert : systemState.activeAlerts) {
                if (alert.severity >= 3) criticalAlerts++;
                else if (alert.severity >= 2) warningAlerts++;
            }
            
            if (criticalAlerts > 0) {
                systemStatus = "Critical";
                statusColor = { 255, 100, 100, 255 }; // Red
            } else if (warningAlerts > 0) {
                systemStatus = "Warning";
                statusColor = { 255, 200, 0, 255 }; // Orange
            }
            
            UICard::RenderMetricCard(clayMan, "System Status",
                systemStatus,
                std::to_string(criticalAlerts) + " critical, " + std::to_string(warningAlerts) + " warnings",
                statusColor, cardHeight);
            
            // Last Alert Time (mock data for now)
            UICard::RenderMetricCard(clayMan, "Last Alert",
                systemState.activeAlerts.empty() ? "None" : "2m ago",
                systemState.activeAlerts.empty() ? "No recent alerts" : "Most recent trigger",
                { 255, 255, 100, 255 }, cardHeight);
        });
        
    }, cardHeight + 80);
}

void AlertsScreen::RenderActiveAlerts(ClayMan* clayMan, const std::vector<SystemAlert>& alerts)
{
    UICard::RenderSectionCard(clayMan, "Active Alerts", [this, clayMan, &alerts]() {
        
        Clay_ElementDeclaration alertsContainer = {};
        alertsContainer.layout.sizing = clayMan->expandXY();
        alertsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        alertsContainer.layout.childGap = 12;
        
        clayMan->element(alertsContainer, [this, clayMan, &alerts]() {
            
            if (alerts.empty()) {
                // No active alerts message
                Clay_ElementDeclaration noAlertsContainer = {};
                noAlertsContainer.layout.sizing = clayMan->expandXfixedY(120);
                noAlertsContainer.layout.childAlignment = clayMan->centerXY();
                noAlertsContainer.backgroundColor = { 25, 50, 25, 255 }; // Subtle green background
                noAlertsContainer.cornerRadius = { 12, 12, 12, 12 };
                
                clayMan->element(noAlertsContainer, [clayMan]() {
                    Clay_ElementDeclaration messageLayout = {};
                    messageLayout.layout.sizing = clayMan->expandXY();
                    messageLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
                    messageLayout.layout.childAlignment = clayMan->centerXY();
                    messageLayout.layout.childGap = 8;
                    
                    clayMan->element(messageLayout, [clayMan]() {
                        // Success icon
                        Clay_ElementDeclaration iconContainer = {};
                        iconContainer.layout.sizing = clayMan->fixedSize(40, 40);
                        iconContainer.backgroundColor = { 0, 255, 150, 255 };
                        iconContainer.cornerRadius = { 20, 20, 20, 20 };
                        iconContainer.layout.childAlignment = clayMan->centerXY();
                        
                        clayMan->element(iconContainer, [clayMan]() {
                            Clay_TextElementConfig iconText = {};
                            iconText.textColor = { 25, 25, 25, 255 };
                            iconText.fontId = 0;
                            iconText.fontSize = 20;
                            clayMan->textElement("?", iconText);
                        });
                        
                        // Message
                        Clay_ElementDeclaration textContainer = {};
                        textContainer.layout.sizing = clayMan->expandXY();
                        textContainer.layout.childAlignment = clayMan->centerXY();
                        
                        clayMan->element(textContainer, [clayMan]() {
                            Clay_TextElementConfig messageText = {};
                            messageText.textColor = { 180, 255, 180, 255 };
                            messageText.fontId = 0;
                            messageText.fontSize = 16;
                            clayMan->textElement("All systems operating normally", messageText);
                        });
                    });
                });
            } else {
                // Sort alerts by severity (highest first)
                std::vector<SystemAlert> sortedAlerts = alerts;
                std::sort(sortedAlerts.begin(), sortedAlerts.end(),
                    [](const SystemAlert& a, const SystemAlert& b) {
                        return a.severity > b.severity;
                    });
                
                for (const auto& alert : sortedAlerts) {
                    RenderAlertCard(clayMan, alert);
                }
            }
        });
        
    }, 0); // Auto height
}

void AlertsScreen::RenderAlertCard(ClayMan* clayMan, const SystemAlert& alert)
{
    // Determine colors based on severity
    Clay_Color severityColor;
    std::string severityText;
    
    switch (alert.severity) {
        case 3:
            severityColor = { 255, 100, 100, 255 }; // Red - Critical
            severityText = "CRITICAL";
            break;
        case 2:
            severityColor = { 255, 200, 0, 255 }; // Orange - Warning
            severityText = "WARNING";
            break;
        default:
            severityColor = { 100, 150, 255, 255 }; // Blue - Info
            severityText = "INFO";
            break;
    }
    
    UICard::RenderWithBackground(clayMan, [clayMan, &alert, severityColor, &severityText]() {
        
        Clay_ElementDeclaration alertLayout = {};
        alertLayout.layout.sizing = clayMan->expandXfixedY(100);
        alertLayout.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        alertLayout.layout.padding = clayMan->padAll(16);
        alertLayout.layout.childGap = 16;
        
        clayMan->element(alertLayout, [clayMan, &alert, severityColor, &severityText]() {
            
            // Severity indicator
            Clay_ElementDeclaration severityIndicator = {};
            severityIndicator.layout.sizing = clayMan->fixedSize(6, 100);
            severityIndicator.backgroundColor = severityColor;
            severityIndicator.cornerRadius = { 3, 3, 3, 3 };
            clayMan->element(severityIndicator, []() {});
            
            // Alert content
            Clay_ElementDeclaration contentSection = {};
            contentSection.layout.sizing = clayMan->expandXY();
            contentSection.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            contentSection.layout.childGap = 8;
            
            clayMan->element(contentSection, [clayMan, &alert, severityColor, &severityText]() {
                
                // Header row with title and severity
                Clay_ElementDeclaration headerRow = {};
                headerRow.layout.sizing = clayMan->expandXfixedY(24);
                headerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
                headerRow.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(headerRow, [clayMan, &alert, severityColor, &severityText]() {
                    // Alert title
                    Clay_ElementDeclaration titleContainer = {};
                    titleContainer.layout.sizing = clayMan->expandXY();
                    titleContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                    
                    clayMan->element(titleContainer, [clayMan, &alert]() {
                        Clay_TextElementConfig titleText = {};
                        titleText.textColor = { 220, 220, 220, 255 };
                        titleText.fontId = 0;
                        titleText.fontSize = 16;
                        clayMan->textElement(alert.title, titleText);
                    });
                    
                    // Severity badge
                    Clay_ElementDeclaration severityBadge = {};
                    severityBadge.layout.sizing = clayMan->fixedSize(80, 20);
                    severityBadge.backgroundColor = severityColor;
                    severityBadge.cornerRadius = { 10, 10, 10, 10 };
                    severityBadge.layout.childAlignment = clayMan->centerXY();
                    
                    clayMan->element(severityBadge, [clayMan, &severityText]() {
                        Clay_TextElementConfig severityTextConfig = {};
                        severityTextConfig.textColor = { 25, 25, 25, 255 };
                        severityTextConfig.fontId = 0;
                        severityTextConfig.fontSize = 10;
                        clayMan->textElement(severityText, severityTextConfig);
                    });
                });
                
                // Alert message
                Clay_ElementDeclaration messageContainer = {};
                messageContainer.layout.sizing = clayMan->expandXfixedY(32);
                messageContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(messageContainer, [clayMan, &alert]() {
                    Clay_TextElementConfig messageText = {};
                    messageText.textColor = { 180, 180, 180, 255 };
                    messageText.fontId = 0;
                    messageText.fontSize = 14;
                    clayMan->textElement(alert.message, messageText);
                });
                
                // Alert details
                Clay_ElementDeclaration detailsRow = {};
                detailsRow.layout.sizing = clayMan->expandXfixedY(20);
                detailsRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
                detailsRow.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                detailsRow.layout.childGap = 16;
                
                clayMan->element(detailsRow, [clayMan, &alert]() {
                    // Current value
                    Clay_TextElementConfig detailText = {};
                    detailText.textColor = { 140, 140, 140, 255 };
                    detailText.fontId = 0;
                    detailText.fontSize = 12;
                    
                    std::string currentValueStr = "Current: " + FormatPercentage(alert.currentValue);
                    clayMan->textElement(currentValueStr, detailText);
                    
                    // Threshold
                    std::string thresholdStr = "Threshold: " + FormatPercentage(alert.threshold);
                    clayMan->textElement(thresholdStr, detailText);
                    
                    // Timestamp (mock for now)
                    clayMan->textElement("Time: Just now", detailText);
                });
            });
        });
        
    }, { 45, 45, 45, 255 }, 100, 0, 12);
}

void AlertsScreen::RenderAlertRules(ClayMan* clayMan, const std::vector<AlertRule>& rules)
{
    UICard::RenderSectionCard(clayMan, "Alert Rules Configuration", [this, clayMan, &rules]() {
        
        Clay_ElementDeclaration rulesContainer = {};
        rulesContainer.layout.sizing = clayMan->expandXY();
        rulesContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        rulesContainer.layout.childGap = 8;
        
        clayMan->element(rulesContainer, [this, clayMan, &rules]() {
            
            if (rules.empty()) {
                // No rules message
                Clay_ElementDeclaration noRulesContainer = {};
                noRulesContainer.layout.sizing = clayMan->expandXfixedY(80);
                noRulesContainer.layout.childAlignment = clayMan->centerXY();
                
                clayMan->element(noRulesContainer, [clayMan]() {
                    Clay_TextElementConfig noRulesText = {};
                    noRulesText.textColor = { 150, 150, 150, 255 };
                    noRulesText.fontId = 0;
                    noRulesText.fontSize = 14;
                    clayMan->textElement("No alert rules configured", noRulesText);
                });
            } else {
                // Rules table header
                RenderRulesTableHeader(clayMan);
                
                // Rules list
                for (const auto& rule : rules) {
                    RenderAlertRuleRow(clayMan, rule);
                }
            }
        });
        
    }, 0); // Auto height
}

void AlertsScreen::RenderRulesTableHeader(ClayMan* clayMan)
{
    Clay_ElementDeclaration headerRow = {};
    headerRow.layout.sizing = clayMan->expandXfixedY(36);
    headerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    headerRow.layout.childGap = 1;
    headerRow.backgroundColor = { 50, 50, 50, 255 };
    headerRow.cornerRadius = { 8, 8, 0, 0 };
    headerRow.layout.padding = clayMan->padXY(12, 8);
    
    clayMan->element(headerRow, [clayMan]() {
        
        // Rule Type
        Clay_ElementDeclaration typeColumn = {};
        typeColumn.layout.sizing = clayMan->expandXfixedY(36);
        typeColumn.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
        clayMan->element(typeColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Rule Type", headerText);
        });
        
        // Threshold
        Clay_ElementDeclaration thresholdColumn = {};
        thresholdColumn.layout.sizing = clayMan->fixedSize(100, 36);
        thresholdColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(thresholdColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Threshold", headerText);
        });
        
        // Duration
        Clay_ElementDeclaration durationColumn = {};
        durationColumn.layout.sizing = clayMan->fixedSize(80, 36);
        durationColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(durationColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Duration", headerText);
        });
        
        // Triggered Count
        Clay_ElementDeclaration countColumn = {};
        countColumn.layout.sizing = clayMan->fixedSize(80, 36);
        countColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(countColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Triggered", headerText);
        });
        
        // Status
        Clay_ElementDeclaration statusColumn = {};
        statusColumn.layout.sizing = clayMan->fixedSize(80, 36);
        statusColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(statusColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Status", headerText);
        });
    });
}

void AlertsScreen::RenderAlertRuleRow(ClayMan* clayMan, const AlertRule& rule)
{
    // Get rule type name
    std::string ruleTypeName;
    switch (rule.type) {
        case AlertRule::CPU_USAGE: ruleTypeName = "CPU Usage"; break;
        case AlertRule::MEMORY_USAGE: ruleTypeName = "Memory Usage"; break;
        case AlertRule::DISK_USAGE: ruleTypeName = "Disk Usage"; break;
        case AlertRule::NETWORK_USAGE: ruleTypeName = "Network Usage"; break;
        case AlertRule::PROCESS_COUNT: ruleTypeName = "Process Count"; break;
        default: ruleTypeName = "Unknown"; break;
    }
    
    Clay_ElementDeclaration ruleRow = {};
    ruleRow.layout.sizing = clayMan->expandXfixedY(32);
    ruleRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    ruleRow.layout.childGap = 1;
    ruleRow.backgroundColor = { 35, 35, 35, 255 };
    ruleRow.layout.padding = clayMan->padXY(12, 6);
    
    clayMan->element(ruleRow, [clayMan, &rule, &ruleTypeName]() {
        
        // Rule Type
        Clay_ElementDeclaration typeCell = {};
        typeCell.layout.sizing = clayMan->expandXfixedY(32);
        typeCell.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
        clayMan->element(typeCell, [clayMan, &ruleTypeName]() {
            Clay_TextElementConfig typeText = {};
            typeText.textColor = { 200, 200, 200, 255 };
            typeText.fontId = 0;
            typeText.fontSize = 12;
            clayMan->textElement(ruleTypeName, typeText);
        });
        
        // Threshold
        Clay_ElementDeclaration thresholdCell = {};
        thresholdCell.layout.sizing = clayMan->fixedSize(100, 32);
        thresholdCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(thresholdCell, [clayMan, &rule]() {
            Clay_TextElementConfig thresholdText = {};
            thresholdText.textColor = { 180, 180, 180, 255 };
            thresholdText.fontId = 0;
            thresholdText.fontSize = 12;
            clayMan->textElement(FormatPercentage(rule.threshold), thresholdText);
        });
        
        // Duration
        Clay_ElementDeclaration durationCell = {};
        durationCell.layout.sizing = clayMan->fixedSize(80, 32);
        durationCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(durationCell, [clayMan, &rule]() {
            Clay_TextElementConfig durationText = {};
            durationText.textColor = { 160, 160, 160, 255 };
            durationText.fontId = 0;
            durationText.fontSize = 12;
            clayMan->textElement(std::to_string(rule.durationSeconds) + "s", durationText);
        });
        
        // Triggered Count
        Clay_ElementDeclaration countCell = {};
        countCell.layout.sizing = clayMan->fixedSize(80, 32);
        countCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(countCell, [clayMan, &rule]() {
            Clay_Color countColor = rule.triggeredCount > 0 ? 
                Clay_Color{255, 150, 0, 255} : Clay_Color{100, 150, 255, 255};
            
            Clay_TextElementConfig countText = {};
            countText.textColor = countColor;
            countText.fontId = 0;
            countText.fontSize = 12;
            clayMan->textElement(std::to_string(rule.triggeredCount), countText);
        });
        
        // Status
        Clay_ElementDeclaration statusCell = {};
        statusCell.layout.sizing = clayMan->fixedSize(80, 32);
        statusCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(statusCell, [clayMan, &rule]() {
            Clay_Color statusColor = rule.isEnabled ? 
                Clay_Color{0, 255, 150, 255} : Clay_Color{255, 100, 100, 255};
            
            Clay_TextElementConfig statusText = {};
            statusText.textColor = statusColor;
            statusText.fontId = 0;
            statusText.fontSize = 12;
            clayMan->textElement(rule.isEnabled ? "Enabled" : "Disabled", statusText);
        });
    });
}