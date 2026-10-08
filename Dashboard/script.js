/*
    IMPORTANT:

    Replace this IP with the IP address
    shown in Arduino Serial Monitor.

    Example:

    ESP32 IP Address: 192.168.1.105

    Then write:

    const ESP32_IP = "192.168.1.105";
*/

const ESP32_IP = "192.168.1.105";

const API_URL =
    `http://${ESP32_IP}/api/data`;


// ================= FETCH DATA =================

async function updateDashboard() {

    try {

        const response =
            await fetch(API_URL);

        if (!response.ok) {

            throw new Error(
                "ESP32 response error"
            );
        }

        const data =
            await response.json();


        // Connection status

        document.getElementById(
            "connectionStatus"
        ).innerText =
            "ESP32 Connected";


        // Current time

        document.getElementById(
            "currentTime"
        ).innerText =
            data.currentTime;


        // Total users

        document.getElementById(
            "totalUsers"
        ).innerText =
            data.users.length;


        // Currently inside

        const insideCount =
            data.users.filter(
                user => user.inside
            ).length;


        document.getElementById(
            "insideUsers"
        ).innerText =
            insideCount;


        // User table

        const table =
            document.getElementById(
                "userTable"
            );

        table.innerHTML = "";


        data.users.forEach(
            user => {

                const row =
                    document.createElement(
                        "tr"
                    );


                const status =
                    user.inside
                    ? "INSIDE"
                    : "OUTSIDE";


                const statusClass =
                    user.inside
                    ? "status-inside"
                    : "status-outside";


                row.innerHTML = `

                    <td>
                        ${user.name}
                    </td>

                    <td class="${statusClass}">
                        ${status}
                    </td>

                    <td>
                        ${user.entryTime}
                    </td>

                    <td>
                        ${user.exitTime}
                    </td>

                `;


                table.appendChild(row);

            }
        );

    }

    catch (error) {

        console.error(error);


        document.getElementById(
            "connectionStatus"
        ).innerText =
            "ESP32 Not Connected";

    }

}


// ================= AUTO REFRESH =================

// Update every 2 seconds

setInterval(
    updateDashboard,
    2000
);


// First update immediately

updateDashboard();
