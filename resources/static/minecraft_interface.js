let player_id = (new URLSearchParams(window.location.search)).get("player_id")

document.getElementById("minecraft-left").addEventListener("click", () => {
	playerMove(player_id, "left")
})
document.getElementById("minecraft-right").addEventListener("click", () => {
	playerMove(player_id, "right")
})
document.getElementById("minecraft-up").addEventListener("click", () => {
	playerMove(player_id, "up")
})
document.getElementById("minecraft-down").addEventListener("click", () => {
	playerMove(player_id, "down")
})

document.getElementById("place-block").addEventListener("click", () => {
	playerPlaceBlock(player_id)
})

let blocks_container = document.getElementById("minecraft-block-selection")
function createBlockEntry(block) {
	let block_entry = document.createElement("li")
	let block_color_square = document.createElement("div")
	block_color_square.classList.add("minecraft-color-block")
	block_entry.appendChild(block_color_square)
	block_entry.appendChild(document.createTextNode(block))
	getBlockColor(block, (block_color) => {
		block_color_square.style.background = block_color

		block_entry.addEventListener("click", () => {
			setPlayerBlock(player_id, block)
		})
	})
	return block_entry
}
getBlocks((blocks) => {
	blocks.forEach((block) => {
		let block_entry = createBlockEntry(block)
		blocks_container.appendChild(block_entry)
	})
})
