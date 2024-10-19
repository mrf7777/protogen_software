let enable_button = document.getElementById("enable-button")
enable_button.addEventListener("click", () => {
	startMinecraft()
})

let seedInput = document.getElementById("seed-input")
document.getElementById("generate-button").addEventListener("click", () => {
    generateWorld(seedInput.value)
})

let player_list = document.getElementById("players-list")
function createPlayerElement(player_id) {
	let player_list_item = document.createElement("li")
	let player_user_interface_link_element = document.createElement("a")
	player_user_interface_link_element.href = `${origin}/protogen/minecraft/interface?player_id=${player_id}`
	player_user_interface_link_element.rel = "noopener noreferrer"
	player_user_interface_link_element.target = "_blank"
	player_user_interface_link_element.appendChild(document.createTextNode("Play as"))
	let player_id_element = document.createTextNode(player_id)
	player_list_item.appendChild(player_user_interface_link_element)
	player_list_item.appendChild(player_id_element)
	return player_list_item
}
function populatePlayerList() {
	getPlayers((players) => {
		player_list.innerHTML = ""
		players.forEach((player) => {
			player_list.appendChild(createPlayerElement(player))
		})
	})
}
populatePlayerList()
setInterval(populatePlayerList, 2000)

let add_player_button = document.getElementById("add-player-button")
add_player_button.addEventListener("click", () => {
	let player_id = window.prompt("Enter new player name.")
	createPlayer(player_id)
})
